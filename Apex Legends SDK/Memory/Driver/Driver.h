#pragma once
#include <Windows.h>

#define IO_KERNEL_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

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

class KernelDriver
{
private:

	uintptr_t currentPID;
	uintptr_t targetPID;
	HANDLE hDriver;

public:

	KernelDriver(LPCSTR driver)
	{
		hDriver = CreateFileA(driver, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	}

	bool SendRequest(PKERNEL_REQUEST pRequest)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		ULONG bytesIO;

		if (DeviceIoControl(hDriver, IO_KERNEL_REQUEST, pRequest, sizeof(KERNEL_REQUEST), pRequest, sizeof(KERNEL_REQUEST), &bytesIO, NULL))
			return true;
		else
			return false;
	}

	uintptr_t GetModuleBase(uintptr_t pID, const wchar_t* modName)
	{
		targetPID = pID;
		currentPID = GetCurrentProcessId();
		KERNEL_REQUEST request;
		request.instructionID = INST_GETBASE;
		request.pID = targetPID;
		request.modName = modName;
		if (!SendRequest(&request))
			return 0;

		uintptr_t base = 0;
		base = reinterpret_cast<uintptr_t>(request.response);
		return base;
	}

	// you can use this to read strings
	bool ReadRaw(UINT_PTR readAddress, UINT_PTR targetAddress, ULONG size)
	{
		KERNEL_REQUEST request;
		request.instructionID = INST_READ;
		request.srcPID = currentPID;
		request.pID = targetPID;
		request.srcAddr = readAddress;
		request.targetAddr = targetAddress;
		request.size = size;
		return SendRequest(&request);
	}

	template<typename type>
	type rpm(uintptr_t readAddress)
	{
		type tmp;
		if (ReadRaw(readAddress, (UINT_PTR)&tmp, sizeof(type)))
			return tmp;
		else
			return { 0 };
	}

	template<typename type>
	bool wpm(UINT_PTR writeAddress, const type& value)
	{
		KERNEL_REQUEST request;
		request.instructionID = INST_WRITE;
		request.srcPID = currentPID;
		request.pID = targetPID;
		request.srcAddr = (UINT_PTR)&value;
		request.targetAddr = writeAddress;
		request.size = sizeof(value);
		return SendRequest(&request);
	}

	bool isrunning()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;
		else
			return true;
	}
};