#include "pch.h"
#include "Beeper.h"
#include "KeyBeep.h"

template<typename T>
inline void SAFE_RELEASE(T* t)
{
	if (t != nullptr)
	{
		t->Release();
	}
}

template<typename T>
inline void SAFE_RELEASE(std::unique_ptr<T> t)
{
	if (t != nullptr)
	{
		(*t).Release();
	}
}

static constexpr uint16_t GetVolume(const size_t frameInd)
{
	constexpr size_t len = 512;

	if (frameInd >= len)
	{
		return 0;
	}
	else
	{
		return static_cast<uint16_t>((len - frameInd) * (len - frameInd) / len / 16);
	}
}

void Beeper::GenerateBeep(const size_t startFrameInd, int16_t* data, const uint32_t frames, const uint32_t channels)
{
	if (mTriggered.exchange(false, std::memory_order_relaxed))
	{
		mLastKeyPressInd = startFrameInd;
	}

	uint16_t startVolume = 0;
	uint16_t endVolume = 0;

	if (mLastKeyPressInd > 0)
	{
		startVolume = GetVolume(startFrameInd - mLastKeyPressInd);
		if (startVolume <= 0)
		{
			endVolume = 0;
			mLastKeyPressInd = 0;
		}
		else
		{
			endVolume = GetVolume(startFrameInd + frames - mLastKeyPressInd);
			if (endVolume <= 0)
			{
				endVolume = 0;
			}
		}
	}

	if (startVolume == 0)
	{
		memset(data, 0, sizeof(data[0]) * frames * channels);
		return;
	}

	for (uint_fast32_t i = 0; i < frames; ++i)
	{

		const size_t currFrameInd = startFrameInd - mLastKeyPressInd + i + 5;
		
		int16_t sawtooth = (static_cast<int16_t>(currFrameInd % 21) - 10);
		sawtooth *= startVolume + static_cast<decltype(sawtooth)>((static_cast<int_fast32_t>(endVolume - startVolume) * i) / frames);

		for (uint_fast32_t j = 0; j < channels; ++j)
		{
			data[i * channels + j] = sawtooth;
		}
	}
}

Beeper::Device::Device(Beeper& owner, IMMDevice* device)
	: mOwner(owner), mMMDevice(device)
{
	Initialize();
}

Beeper::Device::Device(Beeper& owner, IMMDeviceEnumerator* enumerator)
	: mOwner(owner)
{
	HRESULT hRes = S_OK;
	hRes = enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &mMMDevice);

	if (FAILED(hRes))
	{
		std::exit(1);
	}

	Initialize();
}

Beeper::Device::~Device()
{
	Stop();

	SAFE_RELEASE(mAudioRenderClient);
	SAFE_RELEASE(mAudioClient);
	SAFE_RELEASE(mMMDevice);
}

void Beeper::Device::Main()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	const uint32_t numChannels = static_cast<uint32_t>(mMixFormat.Format.nChannels);
	const UINT32 numFrameCount = static_cast<UINT32>(mBufferFrameCount);
	size_t currFrameInd = 0;

	while (mIsRunning.load(std::memory_order_relaxed))
	{
		assert(mAudioClient != nullptr);

		UINT32 currPadding = 0;
		mAudioClient->GetCurrentPadding(&currPadding);

		const UINT32 numFrames = numFrameCount - currPadding;
		if (numFrames == 0)
		{
			continue;
		}

		BYTE* data = nullptr;
		mAudioRenderClient->GetBuffer(numFrames, &data);

		if (data == nullptr)
		{
			continue;
		}

		mOwner.GenerateBeep(currFrameInd, reinterpret_cast<int16_t*>(data), numFrames, numChannels);
		currFrameInd += numFrames;

		mAudioRenderClient->ReleaseBuffer(numFrames, 0);
	}
}

void Beeper::Device::Initialize()
{
	assert(mMMDevice != nullptr);

	HRESULT hRes = S_OK;
	hRes = mMMDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&mAudioClient));

	if (FAILED(hRes))
	{
		std::exit(1);
	}

	WAVEFORMATEX* waveFormat = nullptr;
	hRes = mAudioClient->GetMixFormat(&waveFormat);
	if (FAILED(hRes))
	{
		std::exit(1);
	}

	mMixFormat = *reinterpret_cast<WAVEFORMATEXTENSIBLE*>(waveFormat);

	UINT32 _1, _2, _3;
	mAudioClient->GetSharedModeEnginePeriod(waveFormat, &_1, &_2, reinterpret_cast<UINT32*>(&mBufferFrameCount), &_3);
	CoTaskMemFree(waveFormat);

	InitSampleType();
}

void Beeper::Device::InitSampleType()
{
	mMixFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
	mMixFormat.Format.wBitsPerSample = sizeof(int16_t) * CHAR_BIT;
	mMixFormat.Samples.wValidBitsPerSample = sizeof(int16_t) * CHAR_BIT;

	mMixFormat.Format.nBlockAlign = mMixFormat.Format.nChannels * sizeof(int16_t);
	mMixFormat.Format.nAvgBytesPerSec = mMixFormat.Format.nSamplesPerSec * mMixFormat.Format.nBlockAlign;
}

std::wstring Beeper::Device::GetFriendlyName() const
{
	IPropertyStore* properties;
	const_cast<IMMDevice *>(mMMDevice)->OpenPropertyStore(STGM_READ, &properties);

	PROPVARIANT varName;
	::PropVariantInit(&varName);

	properties->GetValue(PKEY_Device_FriendlyName, &varName);

	return varName.pwszVal;
}

bool Beeper::Device::Start()
{
	HRESULT hRes = S_OK;

	assert(mMMDevice != nullptr);

	mEventHandle = ::CreateEventW(nullptr, false, false, nullptr);
	assert(mEventHandle != nullptr);

	if (mIsRunning.exchange(true) != false)
	{
		return false;
	}

	hRes = mAudioClient->InitializeSharedAudioStream(AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
		static_cast<UINT32>(mBufferFrameCount), &(mMixFormat.Format), nullptr);
	if (FAILED(hRes))
	{
		switch (hRes)
		{
		case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED:
			std::wcout << L"Failed to initialize WASAPI AudioClient (exclusive mode not allowed)." << std::endl;
			break;
		case AUDCLNT_E_BUFFER_SIZE_ERROR:
			std::wcout << L"Failed to initialize WASAPI AudioClient (buffer size error)." << std::endl;
			break;
		default:
			std::wcout << L"Failed to initialize WASAPI AudioClient." << std::endl;
		}
		mIsRunning.store(false);

		return false;
	}

	hRes = mAudioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&mAudioRenderClient));
	if (FAILED(hRes))
	{
		std::wcout << L"Failed to get audio render client of WASAPI AudioClient." << std::endl;
		mIsRunning.store(false);

		return false;
	}
	assert(mAudioRenderClient != nullptr);

	hRes = mAudioClient->GetBufferSize(reinterpret_cast<UINT32*>(&mBufferFrameCount));
	if (FAILED(hRes))
	{
		std::wcout << L"Failed to get the buffer size of WASAPI AudioClient." << std::endl;
		mIsRunning.store(false);

		return false;
	}

	hRes = mAudioClient->SetEventHandle(mEventHandle);
	if (FAILED(hRes))
	{
		std::wcout << L"Failed to set event handle of WASAPI AudioClient." << std::endl;
		mIsRunning.store(false);

		return false;
	}

	hRes = mAudioClient->Start();
	if (FAILED(hRes))
	{
		std::wcout << L"Failed to start WASAPI AudioClient." << std::endl;
		mIsRunning.store(false);

		return false;
	}

	mThread = std::thread(&Beeper::Device::Main, this);

	std::wstringstream stream;
	stream << mMixFormat.Format.nSamplesPerSec << L"Hz sampling rate | "
		<< mBufferFrameCount << L" samples = " << (mBufferFrameCount * 1000) / mMixFormat.Format.nSamplesPerSec << L"ms buffer size";

	mOwner.mOwner.SetMessage(stream.str());

	return true;
}

void Beeper::Device::Stop()
{
	if (mIsRunning.exchange(false) == true)
	{
		if (mThread.joinable())
		{
			mThread.join();
		}

		if (mAudioClient)
		{
			mAudioClient->Stop();
		}

		if (mEventHandle)
		{
			CloseHandle(mEventHandle);
			mEventHandle = nullptr;
		}
	}
}

Beeper::Beeper(KeyBeep& owner)
	: mOwner(owner)
{
	HRESULT hRes;

	hRes = CoInitialize(nullptr);
	if (FAILED(hRes))
	{
		std::exit(1);
	}

	hRes = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID *>(&mDeviceEnumerator));
	if (FAILED(hRes))
	{
		std::exit(1);
	}

	mDevice = std::make_unique<Device>(*this, mDeviceEnumerator);
	std::wcout << L"Device: " << mDevice->GetFriendlyName() << std::endl;
}

Beeper::~Beeper()
{
	if (mDevice)
	{
		mDevice->Stop();
	}

	SAFE_RELEASE(mDeviceEnumerator);
	CoUninitialize();
}