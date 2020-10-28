#pragma once

#include <cassert>

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <mmsystem.h>
#include <conio.h>

#pragma comment(lib, "Winmm.lib")

#include "targetver.h"
#include "Resource.h"