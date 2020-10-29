#pragma once

struct IMMDeviceEnumerator;

class KeyBeep;

class Beeper
{
protected:
	using time_point = std::chrono::steady_clock::time_point;
	
	class Device
	{
	public:
		Device(Beeper& owner, IMMDevice* device);
		Device(Beeper& owner, IMMDeviceEnumerator* enumerator);
		~Device();

		Device(const Device&) = delete;
		Device& operator= (const Device&) = delete;
		Device& operator= (Device&&) = delete;

		inline bool IsRunning() const { return mIsRunning.load(std::memory_order_relaxed); }
		std::wstring GetFriendlyName() const;

		bool Start();
		void Stop();

	protected:
		void Main();
		void Initialize();
		void InitSampleType();

		Beeper& mOwner;

		std::atomic<bool> mIsRunning = false;
		std::thread mThread;

		size_t mBufferFrameCount = 0;

	private:
		IMMDevice* mMMDevice = nullptr;
		IAudioClient3* mAudioClient = nullptr;
		IAudioRenderClient* mAudioRenderClient = nullptr;
		HANDLE mEventHandle = nullptr;
		WAVEFORMATEXTENSIBLE mMixFormat;
	};
public:
	Beeper(KeyBeep& owner);
	~Beeper();

	inline bool Start() { return mDevice != nullptr && mDevice->Start(); }

protected:
	void GenerateBeep(const size_t startFrameInd, int16_t* data, const uint32_t frames, const uint32_t channels);

	KeyBeep& mOwner;
	IMMDeviceEnumerator* mDeviceEnumerator = nullptr;

	std::unique_ptr<Device> mDevice = nullptr;

private:
	time_point mLastKeyPressTime = time_point::min();
	std::atomic<time_point::duration> mLastKeyPressDelay = time_point::duration::zero();
	size_t mLastKeyPressInd = 0;
};