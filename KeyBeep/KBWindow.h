#pragma once

class KBWindow
{
public:
	KBWindow(HINSTANCE hInstance);
	bool InitInstance(int nCmdShow);

	void SetTitle(const std::wstring& str) { mTitle = str; }
	const std::wstring& GetTitle() const noexcept { return mTitle; }

protected:
	void RegisterClass();

	HINSTANCE mInstance = nullptr;
	HWND mWnd = nullptr;

	std::wstring mTitle;
	std::wstring mWindowClass;

private:
	void LoadString(UINT uid, _Out_ std::wstring& out);

	static constexpr size_t LOAD_STRING_BUFFER_SIZE = 128;
	WCHAR mLoadStringBuffer[LOAD_STRING_BUFFER_SIZE] = { L'\0', };
};