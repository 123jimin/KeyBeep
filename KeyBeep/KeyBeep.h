#pragma once
#include "KBWindow.h"

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
	
	void SetLastKeyPress(LastKeyPress&& lastKeyPress) {
		mLastKeyPress.store(lastKeyPress, std::memory_order_relaxed);
	}

protected:
	KBWindow mWindow;
	KBHook& mHook;
	HINSTANCE mInstance = nullptr;

	std::atomic<LastKeyPress> mLastKeyPress;
};
