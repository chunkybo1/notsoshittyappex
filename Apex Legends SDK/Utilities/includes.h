#pragma once
#include <iostream>
#include <Windows.h>
#include "../SDK/Engine/Offsets.h"
#include "../SDK/Engine/Engine.h"
#include "../Memory/Driver/Driver.h"
#include "../Utilities/globals.h"
#include "termcolor.hpp"

#define ARGB(a, r, g, b) D3DCOLOR_ARGB(a, r, g, b)
extern KernelDriver Driver;
