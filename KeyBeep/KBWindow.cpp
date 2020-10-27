#include "pch.h"
#include "KBWindow.h"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

KBWindow::KBWindow(HINSTANCE hInstance)
	: mInstance(hInstance)
{
	LoadString(IDS_APP_TITLE, mTitle);
	LoadString(IDC_KEYBEEP, mWindowClass);

	RegisterClass();
}

bool KBWindow::InitInstance(int nCmdShow)
{
    mWnd = ::CreateWindowW(mWindowClass.data(), mTitle.data(), WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, nullptr, nullptr, mInstance, nullptr);

    if (!mWnd)
    {
        return false;
    }

    ::SetMenu(mWnd, nullptr);
    ::ShowWindow(mWnd, nCmdShow);
    ::UpdateWindow(mWnd);

    return true;
}

void KBWindow::RegisterClass()
{
	WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = mInstance;
    wcex.hIcon = LoadIcon(mInstance, MAKEINTRESOURCE(IDI_KEYBEEP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_KEYBEEP);
    wcex.lpszClassName = mWindowClass.data();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);
}

void KBWindow::LoadString(UINT uid, std::wstring& out)
{
	::LoadStringW(mInstance, uid, mLoadStringBuffer, LOAD_STRING_BUFFER_SIZE);
	out = mLoadStringBuffer;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}