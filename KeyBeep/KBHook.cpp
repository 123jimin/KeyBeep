#include "pch.h"
#include "KBHook.h"
#include "KeyBeep.h"

std::unique_ptr<KBHook> KBHook::INSTANCE = nullptr;
std::once_flag KBHook::INSTANCE_INIT;

KBHook& KBHook::Get(KeyBeep& owner)
{
	std::call_once(INSTANCE_INIT, [&]() {
		KBHook* hook = new KBHook(owner);
		INSTANCE.reset(hook);

		hook->Start();
	});

	__assume(INSTANCE != nullptr);

	return *(INSTANCE.get());
}

KBHook::KBHook(KeyBeep& keyBeep)
	: mOwner(keyBeep)
{
}

KBHook::~KBHook()
{
	if (mThread.joinable())
	{
		mThread.detach();
	}
}

void KBHook::Start()
{
	mThread = std::thread(&KBHook::Main, this);
}

void KBHook::Main()
{
	::SetWindowsHookExW(WH_KEYBOARD_LL, KBHook::HookCallback, mOwner.GetHInstance(), 0);

	MSG msg;
	
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void KBHook::OnKeyEvent(const KBDLLHOOKSTRUCT& kbd_ll)
{
	if (kbd_ll.flags & LLKHF_UP) return;

	mOwner.SetLastKeyPress(KeyBeep::LastKeyPress{std::chrono::steady_clock::now()});
}

LRESULT KBHook::HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	const auto result = CallNextHookEx(nullptr, nCode, wParam, lParam);

	const KBDLLHOOKSTRUCT* kbd_ll = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);

	assert(kbd_ll != nullptr);
	assert(INSTANCE != nullptr);

	if (kbd_ll != nullptr && INSTANCE != nullptr)
	{
		INSTANCE->OnKeyEvent(*kbd_ll);
	}
	else
	{
		__assume(0);
	}

	return result;
}
