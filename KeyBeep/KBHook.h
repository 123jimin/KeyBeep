#pragma once

class KeyBeep;

class KBHook
{
public:
	~KBHook();

	static KBHook& Get(KeyBeep& owner);

protected:
	KBHook(KeyBeep& owner);

	void Start();

	void Main();
	void OnKeyEvent(const KBDLLHOOKSTRUCT* kbd_ll);

	KeyBeep& mOwner;
	std::thread mThread;

private:
	__declspec(dllexport) static LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam);

	static std::unique_ptr<KBHook> INSTANCE;
	static std::once_flag INSTANCE_INIT;
};