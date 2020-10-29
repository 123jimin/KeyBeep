#include "pch.h"
#include "Beeper.h"

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

Beeper::Device::Device(IMMDevice* device)
	: mMMDevice(device)
{
	Initialize();
}

Beeper::Device::Device(IMMDeviceEnumerator* enumerator)
{
	HRESULT hRes = S_OK;
	hRes = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &mMMDevice);

	if (FAILED(hRes))
	{
		std::exit(1);
	}

	Initialize();
}

Beeper::Device::~Device()
{
	Stop();

	SAFE_RELEASE(mMMDevice);
}

void Beeper::Device::Main()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	while (mIsRunning.load(std::memory_order_relaxed))
	{
		assert(mAudioClient != nullptr);

		// TODO
	}
}

void Beeper::Device::Initialize()
{
	assert(mMMDevice != nullptr);

	HRESULT hRes = S_OK;
	hRes = mMMDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&mAudioClient));

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

	mMixFormat = *reinterpret_cast<WAVEFORMATEXTENSIBLE *>(waveFormat);
	CoTaskMemFree(waveFormat);
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

void Beeper::Device::Start()
{
	HRESULT hRes = S_OK;

	assert(mMMDevice != nullptr);

	mEventHandle = ::CreateEventW(nullptr, false, false, nullptr);
	assert(mEventHandle != nullptr);

	// TODO

	hRes = mAudioClient->SetEventHandle(mEventHandle);
	if (FAILED(hRes))
	{
		return;
	}

	hRes = mAudioClient->Start();
	if (FAILED(hRes))
	{
		return;
	}

	if (mIsRunning.exchange(true) != false)
	{
		return;
	}

	mThread = std::thread(&Beeper::Device::Main, this);
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

	mDevice = std::make_unique<Device>(mDeviceEnumerator);
	std::wcout << L"Device: " << mDevice->GetFriendlyName() << std::endl;
}

Beeper::~Beeper()
{
	SAFE_RELEASE(mDeviceEnumerator);
	CoUninitialize();
}
