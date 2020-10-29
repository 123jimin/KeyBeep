#pragma once

class KeyBeep;

class KBWindow
{
public:
	KBWindow(KeyBeep& owner);
	~KBWindow();

	bool InitInstance(int nCmdShow);

	inline void SetTitle(const std::wstring& str) { mTitle = str; }
	inline const std::wstring& GetTitle() const noexcept { return mTitle; }

	inline void SetMsg(const std::wstring& str) { mMessage = str; }
	inline const std::wstring& GetMsg() const noexcept { return mMessage; }

protected:
	void Render();
	void RegisterClass();

	KeyBeep& mOwner;

	HINSTANCE mInstance = nullptr;
	HWND mWnd = nullptr;

	std::wstring mTitle;
	std::wstring mWindowClass;
	std::wstring mMessage;

private:
	void LoadString(UINT uid, _Out_ std::wstring& out);

	static constexpr size_t LOAD_STRING_BUFFER_SIZE = 128;
	WCHAR mLoadStringBuffer[LOAD_STRING_BUFFER_SIZE] = { L'\0', };
};