#include "pch.h"
#include "KBWindow.h"
#include "KeyBeep.h"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

KBWindow::KBWindow(KeyBeep& owner)
	: mOwner(owner), mInstance(owner.GetHInstance())
{
	LoadString(IDS_APP_TITLE, mTitle);
	LoadString(IDC_KEYBEEP, mWindowClass);

	RegisterClass();
}

KBWindow::~KBWindow()
{
    ::DestroyWindow(mWnd);
}

bool KBWindow::InitInstance(int nCmdShow)
{
    mWnd = ::CreateWindowW(mWindowClass.data(), mTitle.data(), WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 80, nullptr, nullptr, mInstance, this);

    if (!mWnd)
    {
        return false;
    }

    ::SetMenu(mWnd, nullptr);
    ::ShowWindow(mWnd, nCmdShow);
    ::UpdateWindow(mWnd);

    return true;
}

void KBWindow::Render()
{
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
    case WM_CREATE:
    {
        KBWindow* window = reinterpret_cast<KBWindow*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        break;
    }
    case WM_KEYDOWN:
        break;
    case WM_PAINT:
    {
        KBWindow* window = reinterpret_cast<KBWindow *>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOutW(hdc, 25, 10, window->GetMsg().c_str(), static_cast<int>(window->GetMsg().size()));
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}