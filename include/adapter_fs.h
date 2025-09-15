#ifndef ADAPTER_FS_H
#define ADAPTER_FS_H

#include <stdint.h>
#include <stdio.h>

#ifdef WIN32_BUILD
#include <windows.h>
#else
#include "windows_compat.h"
#endif

// PROV: Evidence sources from runtime.apis.json
// CreateFileA: VA_0x401000+, IAT_0x4c4c20 - main file opening function
// ReadFile: VA_0x401000+, IAT_0x4c4c2c - file reading operations
// SetFilePointer: VA_0x401000+, IAT_0x4c4c38 - file seeking operations

// File system adapter functions
// PROV: CreateFileA call sites for loading assets from Sdata/ directory  
HANDLE adapter_CreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
);

// PROV: ReadFile call sites for asset data loading
BOOL adapter_ReadFile(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
);

// PROV: WriteFile for save game functionality (if used)
BOOL adapter_WriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
);

// PROV: SetFilePointer call sites for seeking within asset files
DWORD adapter_SetFilePointer(
    HANDLE hFile,
    LONG lDistanceToMove,
    PLONG lpDistanceToMoveHigh,
    DWORD dwMoveMethod
);

// File information and management
DWORD adapter_GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
BOOL adapter_CloseHandle(HANDLE hObject);

// Directory operations for asset discovery  
// PROV: FindFirstFileA/FindNextFileA for scanning Sdata/ directory
HANDLE adapter_FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
BOOL adapter_FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
BOOL adapter_FindClose(HANDLE hFindFile);

// DLL loading operations
// PROV: LoadLibraryA call sites for data00.dll-data31.dll loading
HMODULE adapter_LoadLibraryA(LPCSTR lpLibFileName);

// PROV: GetProcAddress for DLL function lookups
FARPROC adapter_GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

// PROV: FreeLibrary for DLL cleanup
BOOL adapter_FreeLibrary(HMODULE hLibModule);

// File dialogs (if used by game)
// PROV: GetOpenFileNameA/GetSaveFileNameA call sites
BOOL adapter_GetOpenFileNameA(LPOPENFILENAMEA lpofn);
BOOL adapter_GetSaveFileNameA(LPOPENFILENAMEA lpofn);

// Path utilities
DWORD adapter_GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer);
BOOL adapter_SetCurrentDirectoryA(LPCSTR lpPathName);
DWORD adapter_GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

// Utility functions
int adapter_fs_init(void);
void adapter_fs_shutdown(void);

// Cross-platform path handling
void adapter_normalize_path(char *path);
int adapter_file_exists(const char *path);

// VFS functions for resource-faithful diskless runtime
// PROV: resources.manifest.json - evidence-based asset resolution
HANDLE adapter_fs_open(const char* path, DWORD access, DWORD creation);
int adapter_fs_exists(const char* path);

// Constants (only define if not already defined in windows_compat.h)
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE    ((HANDLE)(uintptr_t)-1)
#endif

#ifndef INVALID_FILE_SIZE
#define INVALID_FILE_SIZE       ((DWORD)0xFFFFFFFF)
#endif

// File access modes (only define if not already defined)
#ifndef GENERIC_READ
#define GENERIC_READ            0x80000000
#endif

#ifndef GENERIC_WRITE
#define GENERIC_WRITE           0x40000000
#endif

// File share modes (only define if not already defined)
#ifndef FILE_SHARE_READ
#define FILE_SHARE_READ         0x00000001
#endif

#ifndef FILE_SHARE_WRITE
#define FILE_SHARE_WRITE        0x00000002
#endif

// File creation disposition (only define if not already defined)
#ifndef CREATE_NEW
#define CREATE_NEW              1
#define CREATE_ALWAYS           2
#define OPEN_EXISTING           3
#define OPEN_ALWAYS             4
#define TRUNCATE_EXISTING       5
#endif

// File attributes (only define if not already defined)
#ifndef FILE_ATTRIBUTE_NORMAL
#define FILE_ATTRIBUTE_NORMAL   0x00000080
#endif

// Seek methods (only define if not already defined)
#ifndef FILE_BEGIN
#define FILE_BEGIN              0
#define FILE_CURRENT            1
#define FILE_END                2
#endif

#endif // ADAPTER_FS_H