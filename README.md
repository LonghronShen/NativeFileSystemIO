# Native File System IO
Providing a way to read files on an NTFS volume at cluster level.

BOOL WINAPI FileCopy(PCHAR lpSrcName, PCHAR lpDstName);

You can use this main API to copy a file, even the file is locked by other programs.

Note: 
	This function cannot access files which are stored in the MFT directly.