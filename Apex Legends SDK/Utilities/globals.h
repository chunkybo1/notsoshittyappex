#pragma once
#include <Windows.h>
#include "includes.h"
namespace Globals {
	HWND hWnd = NULL;
	uintptr_t modBase = NULL;
	uintptr_t processID = NULL;
	int killKey = VK_END;
}

namespace MenuSettings {
	bool espEnabled = false;
	bool glowEnabled = false;
	bool noRecoilEnabled = false;
	bool aimbotEnabled = false;
	int aimbotSmoothness = 3;
	bool healthbarEnabled = false;
	bool fovCircle = false;
	int aimbotFOV = 300;
	bool snaplinesEnabled = false;
	bool userScriptsEnabled = false;
}
uintptr_t GetPid(const wchar_t* procName);
KernelDriver Driver("\\\\.\\ChunkyDrv");