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

	hRes = mDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &mDevice);
	if (FAILED(hRes))
	{
		std::exit(1);
	}

	IPropertyStore* properties;
	mDevice->OpenPropertyStore(STGM_READ, &properties);

	PROPVARIANT varName;
	::PropVariantInit(&varName);

	properties->GetValue(PKEY_Device_FriendlyName, &varName);

	std::wcout << L"Audio Device: " << varName.pwszVal << std::endl;
}

Beeper::~Beeper()
{
	SAFE_RELEASE(mDeviceEnumerator);
	SAFE_RELEASE(mDevice);
	CoUninitialize();
}
