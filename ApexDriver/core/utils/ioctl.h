#pragma once
#include <ntifs.h>
#include "structs.h"
#include "globals.h"

#define IO_KERNEL_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

BOOLEAN IsValidAddr(ULONG64 ptr);
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp);
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp);

enum
{
	INST_GETBASE = 1,
	INST_READ,
	INST_WRITE
};

typedef struct
{
	ULONG instructionID;
	ULONG srcPID;
	ULONG pID;
	UINT_PTR srcAddr;
	UINT_PTR targetAddr;
	ULONG size;
	const wchar_t* modName;
	PVOID response;
} KERNEL_REQUEST, * PKERNEL_REQUEST;