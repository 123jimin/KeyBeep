#pragma once

#include <winsdkver.h>

#ifdef  _WIN32_WINNT
#undef  _WIN32_WINNT
#endif

// _WIN32_WINNT_WIN7
#define _WIN32_WINNT 0x0601

#include <SDKDDKVer.h>
