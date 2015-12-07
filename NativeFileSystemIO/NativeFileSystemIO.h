// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NATIVEFILESYSTEMIO_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NATIVEFILESYSTEMIO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef NATIVEFILESYSTEMIO_EXPORTS
#define NATIVEFILESYSTEMIO_API __declspec(dllexport)
#else
#define NATIVEFILESYSTEMIO_API __declspec(dllimport)
#endif

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <Windows.h>
#include <Winioctl.h>

ULONGLONG* WINAPI GetFileClusters(PCHAR lpFileName, ULONG ClusterSize, ULONG *ClCount, ULONG *FileSize);

BOOL WINAPI FileCopy(PCHAR lpSrcName, PCHAR lpDstName);

int WINAPI GetProgramBits();

int WINAPI GetSystemBits();