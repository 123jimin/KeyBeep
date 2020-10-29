#include "pch.h"
#include "KeyBeep.h"
#include "KBHook.h"

constexpr size_t MAX_LOADSTRING = 100;

KeyBeep::KeyBeep(HINSTANCE hInstance)
    : mInstance(hInstance),
    mHook(KBHook::Get(*this)), mWindow(*this), mBeeper(*this)
{
}

int KeyBeep::Start(int nCmdShow)
{
    if (!mWindow.InitInstance(nCmdShow))
    {
        return FALSE;
    }

    if (!mBeeper.Start())
    {
        return FALSE;
    }

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (AllocConsole())
    {
        freopen("CONOUT$", "w", stdout);
    }

    setlocale(LC_ALL, ".UTF-8");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout.setf(std::ios::unitbuf);

    KeyBeep keyBeep(hInstance);
    return keyBeep.Start(nCmdShow);
}