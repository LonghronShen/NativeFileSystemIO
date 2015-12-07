// NativeFileSystemIO.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NativeFileSystemIO.h"

#include <exception>

using namespace std;

#pragma comment(lib, "User32.lib")

VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
{
	if (NULL == lpSystemInfo)    return;
	typedef VOID(WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetNativeSystemInfo");;
	if (NULL != fnGetNativeSystemInfo)
	{
		fnGetNativeSystemInfo(lpSystemInfo);
	}
	else
	{
		GetSystemInfo(lpSystemInfo);
	}
}

ULONGLONG* WINAPI GetFileClusters(PCHAR lpFileName, ULONG ClusterSize, ULONG *ClCount, ULONG *FileSize)
{
	try
	{
		HANDLE hFile;
		ULONG OutSize;
		ULONG Bytes, Cls, CnCount, r;
		ULONGLONG *Clusters = NULL;
		BOOLEAN Result = FALSE;
		LARGE_INTEGER PrevVCN, Lcn;
		STARTING_VCN_INPUT_BUFFER InBuf;
		PRETRIEVAL_POINTERS_BUFFER OutBuf;
		hFile = CreateFile(lpFileName, FILE_READ_ATTRIBUTES,
			FILE_SHARE_READ | FILE_SHARE_WRITE |
			FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, 0, 0);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			*FileSize = GetFileSize(hFile, NULL);
			OutSize = sizeof(RETRIEVAL_POINTERS_BUFFER) + (*FileSize / ClusterSize) * sizeof(OutBuf->Extents);
			//OutBuf = (PRETRIEVAL_POINTERS_BUFFER)malloc(OutSize);
			OutBuf = new RETRIEVAL_POINTERS_BUFFER[OutSize];
			InBuf.StartingVcn.QuadPart = 0;

			if (DeviceIoControl(hFile, FSCTL_GET_RETRIEVAL_POINTERS, &InBuf, sizeof(InBuf), OutBuf, OutSize, &Bytes, NULL))
			{
				*ClCount = (*FileSize + ClusterSize - 1) / ClusterSize;
				//Clusters = (ULONGLONG*)malloc(*ClCount * sizeof(ULONGLONG));
				Clusters = new ULONGLONG[*ClCount * sizeof(ULONGLONG)];
				PrevVCN = OutBuf->StartingVcn;
				for (r = 0, Cls = 0; r < OutBuf->ExtentCount; r++)
				{
					Lcn = OutBuf->Extents[r].Lcn;
					for (CnCount = OutBuf->Extents[r].NextVcn.QuadPart - PrevVCN.QuadPart; CnCount; CnCount--, Cls++, Lcn.QuadPart++)
					{
						Clusters[Cls] = Lcn.QuadPart;
					}
					PrevVCN = OutBuf->Extents[r].NextVcn;
				}
			}

			//free(OutBuf);
			delete[] OutBuf;
			CloseHandle(hFile);
		}
		return Clusters;
	}
	catch (...)
	{
#if _DEBUG
		MessageBox(NULL, "Error!", "Error!", MB_OK);
#endif
		return NULL;
	}
}

BOOL WINAPI FileCopy(PCHAR lpSrcName, PCHAR lpDstName)
{
	ULONG ClusterSize, BlockSize;
	ULONGLONG *Clusters;
	ULONG ClCount, FileSize, Bytes;
	HANDLE hDrive, hFile;
	ULONG SecPerCl, BtPerSec, r;
	PVOID Buff;
	LARGE_INTEGER Offset;
	CHAR Name[7];

	Name[0] = lpSrcName[0];
	Name[1] = ':';
	Name[2] = 0;
	GetDiskFreeSpace(Name, &SecPerCl, &BtPerSec, NULL, NULL);
	ClusterSize = SecPerCl * BtPerSec;

	Clusters = GetFileClusters(lpSrcName, ClusterSize, &ClCount, &FileSize);
	if (Clusters)
	{
		Name[0] = '\\';
		Name[1] = '\\';
		Name[2] = '.';
		Name[3] = '\\';
		Name[4] = lpSrcName[0];
		Name[5] = ':';
		Name[6] = 0;
		hDrive = CreateFile(Name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		if (hDrive != INVALID_HANDLE_VALUE)
		{
			hFile = CreateFile(lpDstName, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, 0);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				//Buff = malloc(ClusterSize);
				Buff = new char[ClusterSize];
				for (r = 0; r < ClCount; r++, FileSize -= BlockSize)
				{
					Offset.QuadPart = ClusterSize * Clusters[r];
					SetFilePointer(hDrive, Offset.LowPart, &Offset.HighPart, FILE_BEGIN);
					ReadFile(hDrive, Buff, ClusterSize, &Bytes, NULL);
					BlockSize = FileSize < ClusterSize ? FileSize : ClusterSize;
					WriteFile(hFile, Buff, BlockSize, &Bytes, NULL);
				}
				//free(Buff);
				delete[] Buff;
				CloseHandle(hFile);
			}
			CloseHandle(hDrive);
		}
		//free(Clusters);
		delete[] Clusters;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int WINAPI GetProgramBits()
{
	return sizeof(int*) * 8;
}

int WINAPI GetSystemBits()
{
	SYSTEM_INFO si;
	SafeGetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		return 64;
	}
	return 32;
}