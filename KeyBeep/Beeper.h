#pragma once

struct IMMDeviceEnumerator;

class KeyBeep;

class Beeper
{
protected:
	class Device
	{
	public:
		Device(IMMDevice* device);
		Device(IMMDeviceEnumerator* enumerator);
		~Device();

		Device(const Device&) = delete;
		Device& operator= (const Device&) = delete;
		Device& operator= (Device&&) = delete;

		inline bool IsRunning() const { return mIsRunning.load(std::memory_order_relaxed); }
		std::wstring GetFriendlyName() const;

		void Start();
		void Stop();

	protected:
		void Main();
		void Initialize();

		std::atomic<bool> mIsRunning = false;
		std::thread mThread;

		IMMDevice* mMMDevice = nullptr;
		IAudioClient* mAudioClient = nullptr;
		IAudioRenderClient* mAudioRenderClient = nullptr;
		HANDLE mEventHandle = nullptr;
		WAVEFORMATEXTENSIBLE mMixFormat;
	};
public:
	Beeper(KeyBeep& owner);
	~Beeper();

protected:
	KeyBeep& mOwner;
	IMMDeviceEnumerator* mDeviceEnumerator = nullptr;

	std::unique_ptr<Device> mDevice = nullptr;
};