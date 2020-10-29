#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <cassert>

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <thread>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <audioclient.h>
#include <audiopolicy.h>
#include <avrt.h>
#include <fcntl.h>
#include <functiondiscoverykeys.h>
#include <io.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <objbase.h>
#include <propvarutil.h>

#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "winmm.lib")

#include "targetver.h"
#include "Resource.h"