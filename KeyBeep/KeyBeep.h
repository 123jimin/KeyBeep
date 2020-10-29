#pragma once
#include "KBWindow.h"
#include "Beeper.h"

class KBHook;

class KeyBeep
{
public:
	struct LastKeyPress
	{
		using time_point = std::chrono::steady_clock::time_point;
		time_point mTime = time_point::min();
	};

	KeyBeep(HINSTANCE hInstance);
	int Start(int nCmdShow);

	inline HINSTANCE GetHInstance() const noexcept { return mInstance; }
	
	inline void SetLastKeyPress(LastKeyPress&& lastKeyPress)
	{
		mLastKeyPress.store(lastKeyPress, std::memory_order_relaxed);
	}

	inline LastKeyPress::time_point GetLastKeyPressTime() const
	{
		return mLastKeyPress.load(std::memory_order_relaxed).mTime;
	}

protected:
	KBWindow mWindow;
	KBHook& mHook;
	HINSTANCE mInstance = nullptr;

	Beeper mBeeper;

	std::atomic<LastKeyPress> mLastKeyPress;
};
