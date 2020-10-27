#pragma once
#include "KBWindow.h"

class KeyBeep
{
public:
	KeyBeep(HINSTANCE hInstance);
	int Start(int nCmdShow);

protected:
	KBWindow mWindow;
};
