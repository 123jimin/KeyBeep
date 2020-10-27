#include "pch.h"
#include "KeyBeep.h"

constexpr size_t MAX_LOADSTRING = 100;

KeyBeep::KeyBeep(HINSTANCE hInstance)
    : mWindow(hInstance)
{
}

int KeyBeep::Start(int nCmdShow)
{
    if (!mWindow.InitInstance(nCmdShow))
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

    KeyBeep keyBeep(hInstance);
    return keyBeep.Start(nCmdShow);
}