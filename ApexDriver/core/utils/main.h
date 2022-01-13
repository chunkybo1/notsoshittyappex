#pragma once
#include <ntifs.h>
#include "globals.h"
#include "ioctl.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);