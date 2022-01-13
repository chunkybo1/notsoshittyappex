#include "ioctl.h"

#pragma warning (disable : 4022 4024 4047)

NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

NTKERNELAPI PPEB NTAPI PsGetProcessPeb
(
	IN PEPROCESS Process
);

BOOLEAN IsValidAddr(ULONG64 ptr)
{
	ULONG64 min = 0x0001000;
	ULONG64 max = 0x7FFFFFFEFFFF;
	BOOLEAN result = (ptr > min && ptr < max);
	return result;
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	ULONG bytesIO = 0;
	PIO_STACK_LOCATION stackLoc = IoGetCurrentIrpStackLocation(irp);

	if (stackLoc->Parameters.DeviceIoControl.IoControlCode == IO_KERNEL_REQUEST)
	{
		PKERNEL_REQUEST request = (PKERNEL_REQUEST)irp->AssociatedIrp.SystemBuffer;

		switch (request->instructionID)
		{
		case INST_GETBASE:
		{
			PEPROCESS process = 0;
			PVOID result = 0;

			if (NT_SUCCESS(PsLookupProcessByProcessId(request->pID, &process)))
			{
				PPEB64 peb = (PPEB64)PsGetProcessPeb(process);

				if (peb > 0)
				{
					PVOID buf = ExAllocatePool(NonPagedPool, wcslen(request->modName) * sizeof(wchar_t) + 1);
					if (buf != NULL)
					{
						memcpy(buf, request->modName, wcslen(request->modName) * sizeof(wchar_t) + 1);

						KAPC_STATE state;
						KeStackAttachProcess(process, &state);

						// InLoadOrderLinks will have main executable first, ntdll.dll second, kernel32.dll
						for (PLIST_ENTRY pListEntry = peb->Ldr->InLoadOrderLinks.Flink; pListEntry != &peb->Ldr->InLoadOrderLinks; pListEntry = pListEntry->Flink)
						{
							if (!pListEntry)
								continue;

							PLDR_DATA_TABLE_ENTRY module_entry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

							UNICODE_STRING unicode_name;
							RtlInitUnicodeString(&unicode_name, buf);

							if (RtlCompareUnicodeString(&module_entry->BaseDllName, &unicode_name, TRUE) == 0)
								result = module_entry->DllBase;
						}

						KeUnstackDetachProcess(&state);

						request->response = result;
					}
				}
			}
			break;
		}

		case INST_READ:
		{
			SIZE_T tmp = 0;
			PEPROCESS proc;
			PEPROCESS srcProc;
			if (NT_SUCCESS(PsLookupProcessByProcessId(request->pID, &proc)) && NT_SUCCESS(PsLookupProcessByProcessId(request->srcPID, &srcProc)) && IsValidAddr(request->srcAddr) && IsValidAddr(request->targetAddr))
				MmCopyVirtualMemory(proc, (PVOID)request->srcAddr, srcProc, (PVOID)request->targetAddr, request->size, KernelMode, &tmp);

			break;
		}

		case INST_WRITE:
		{
			SIZE_T tmp = 0;
			PEPROCESS proc;
			PEPROCESS srcProc;
			if (NT_SUCCESS(PsLookupProcessByProcessId(request->pID, &proc)) && NT_SUCCESS(PsLookupProcessByProcessId(request->srcPID, &srcProc)) && IsValidAddr(request->targetAddr) && IsValidAddr(request->srcAddr))
				MmCopyVirtualMemory(srcProc, (PVOID)request->srcAddr, proc, (PVOID)request->targetAddr, request->size, KernelMode, &tmp);

			break;
		}

		default:
			break;
		}

		bytesIO = sizeof(*request);
	}

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = bytesIO;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}