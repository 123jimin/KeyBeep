#pragma once
#include "KBWindow.h"
#include "Beeper.h"

class KBHook;

class KeyBeep
{
public:
	KeyBeep(HINSTANCE hInstance);
	int Start(int nCmdShow);

	inline HINSTANCE GetHInstance() const noexcept { return mInstance; }
	inline void Beep() { mBeeper.Trigger(); }
	inline void SetMessage(const std::wstring& message) { mWindow.SetMsg(message); }
	
protected:
	KBWindow mWindow;
	KBHook& mHook;
	HINSTANCE mInstance = nullptr;

	Beeper mBeeper;
};
