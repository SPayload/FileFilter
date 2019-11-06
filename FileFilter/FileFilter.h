#pragma once

#ifndef __PASSTHROUGH_H__
#define __PASSTHROUGH_H__

#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(g_TraceFlags,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry(
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
);

NTSTATUS
PtUnload(
    __in FLT_FILTER_UNLOAD_FLAGS Flags
);

FLT_POSTOP_CALLBACK_STATUS
PtPostDirCtrlPassThrough(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in_opt PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags
);


NTSTATUS
SpyConnect(
    __in PFLT_PORT ClientPort,
    __in PVOID ServerPortCookie,
    __in_bcount(SizeOfContext) PVOID ConnectionContext,
    __in ULONG SizeOfContext,
    __deref_out_opt PVOID* ConnectionCookie
);

VOID
SpyDisconnect(
    __in_opt PVOID ConnectionCookie
);

NTSTATUS
SpyMessage(
    __in PVOID ConnectionCookie,
    __in_bcount_opt(InputBufferSize) PVOID InputBuffer,
    __in ULONG InputBufferSize,
    __out_bcount_part_opt(OutputBufferSize, *ReturnOutputBufferLength) PVOID OutputBuffer,
    __in ULONG OutputBufferSize,
    __out PULONG ReturnOutputBufferLength
);

NTSTATUS
FileMonGetFullPathName(
    PFLT_FILE_NAME_INFORMATION nameInfo,
    WCHAR* fullpathname
);

void RemoveBacklash(wchar_t* szFileName);

#endif
