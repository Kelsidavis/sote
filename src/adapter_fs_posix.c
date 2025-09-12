#include "adapter_fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dlfcn.h>

/*
 * [RESOURCE] File System Adapter Module
 * PROV: va=0x401000+, api=CreateFileA/ReadFile/WriteFile, xref_count=50+, strings=["Sdata/", "*.bmp", "*.wav", "*.dll"]
 * RESOURCE: 277 game assets (43 BMP, 183 WAV, 17 SAN, 34 DLL) from Sdata/
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c (POSIX implementation)
 * NOTE: Cross-platform file I/O replacing Win32 APIs
 */
// PROV: File I/O API replacements based on runtime.apis.json analysis
// Evidence: CreateFileA at IAT_0x4c4c20 for asset loading from Sdata/
// Evidence: ReadFile at IAT_0x4c4c2c for reading asset data  
// Evidence: SetFilePointer at IAT_0x4c4c38 for seeking in files

// Internal file handle structure
typedef struct {
    FILE *file;
    char *path;
    DWORD access;
    DWORD creation;
    DWORD attributes;
} FILE_HANDLE_INTERNAL;

// Directory enumeration structure
typedef struct {
    DIR *dir;
    char *pattern;
    struct dirent *current;
} FIND_HANDLE_INTERNAL;

/*
 * [RESOURCE] CreateFileA Adapter
 * PROV: va=0x401000+, api=CreateFileA, xref_count=15, strings=["Sdata/", "*.bmp", "*.wav", "*.dll"]
 * RESOURCE: Opens game assets - BMPs (43), WAVs (183), DLLs (34), SANs (17)
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Maps Win32 CreateFileA to POSIX fopen
 */
// PROV: CreateFileA call sites at VA_0x401000+, IAT_0x4c4c20
// Used for opening assets from Sdata/ directory (277 files total)
HANDLE adapter_CreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{
    if (!lpFileName) return INVALID_HANDLE_VALUE;
    
    FILE_HANDLE_INTERNAL *handle = calloc(1, sizeof(FILE_HANDLE_INTERNAL));
    if (!handle) return INVALID_HANDLE_VALUE;
    
    // Normalize Windows path separators to POSIX
    char *normalized_path = strdup(lpFileName);
    adapter_normalize_path(normalized_path);
    
    const char *mode;
    if (dwDesiredAccess & GENERIC_WRITE) {
        if (dwCreationDisposition == CREATE_ALWAYS || dwCreationDisposition == CREATE_NEW) {
            mode = "wb";
        } else if (dwCreationDisposition == TRUNCATE_EXISTING) {
            mode = "wb";
        } else {
            mode = "r+b";
        }
    } else {
        mode = "rb";
    }
    
#ifdef RESOURCE_WARNINGS
    // Runtime check for missing resources
    if (strstr(normalized_path, "Sdata/") || strstr(normalized_path, "sdata/")) {
        struct stat st;
        if (stat(normalized_path, &st) != 0) {
            fprintf(stderr, "[RESWARN] CreateFileA: Missing resource file: %s\n", normalized_path);
        }
    }
#endif
    handle->file = fopen(normalized_path, mode);
    if (!handle->file) {
        free(normalized_path);
        free(handle);
        return INVALID_HANDLE_VALUE;
    }
    
    handle->path = normalized_path;
    handle->access = dwDesiredAccess;
    handle->creation = dwCreationDisposition;
    handle->attributes = dwFlagsAndAttributes;
    
    return (HANDLE)handle;
}

/*
 * [RESOURCE] ReadFile Adapter
 * PROV: va=0x401000+, api=ReadFile, xref_count=20, strings=["Loading", "Read"]
 * RESOURCE: Reads binary data from BMPs, WAVs, DLLs (up to 921KB per file)
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Maps Win32 ReadFile to POSIX fread
 */
// PROV: ReadFile call sites at VA_0x401000+, IAT_0x4c4c2c
// Core function for loading asset data (BMPs, WAVs, DLLs)
BOOL adapter_ReadFile(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped)
{
    if (hFile == INVALID_HANDLE_VALUE || !lpBuffer) return FALSE;
    
    FILE_HANDLE_INTERNAL *handle = (FILE_HANDLE_INTERNAL*)hFile;
    if (!handle->file) return FALSE;
    
    size_t bytes_read = fread(lpBuffer, 1, nNumberOfBytesToRead, handle->file);
    
    if (lpNumberOfBytesRead) {
        *lpNumberOfBytesRead = (DWORD)bytes_read;
    }
    
    return (bytes_read > 0 || nNumberOfBytesToRead == 0);
}

/*
 * [RESOURCE] WriteFile Adapter
 * PROV: va=0x401000+, api=WriteFile, xref_count=3, strings=["Save", "Write"]
 * RESOURCE: Writes save game data and configuration
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Maps Win32 WriteFile to POSIX fwrite
 */
// PROV: WriteFile for potential save game functionality
BOOL adapter_WriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped)
{
    if (hFile == INVALID_HANDLE_VALUE || !lpBuffer) return FALSE;
    
    FILE_HANDLE_INTERNAL *handle = (FILE_HANDLE_INTERNAL*)hFile;
    if (!handle->file) return FALSE;
    
    size_t bytes_written = fwrite(lpBuffer, 1, nNumberOfBytesToWrite, handle->file);
    
    if (lpNumberOfBytesWritten) {
        *lpNumberOfBytesWritten = (DWORD)bytes_written;
    }
    
    return (bytes_written == nNumberOfBytesToWrite);
}

/*
 * [RESOURCE] SetFilePointer Adapter
 * PROV: va=0x401000+, api=SetFilePointer, xref_count=8, strings=["Seek", "Position"]
 * RESOURCE: Seeks within BMP/WAV headers and DLL sections
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Maps Win32 SetFilePointer to POSIX fseek
 */
// PROV: SetFilePointer call sites at VA_0x401000+, IAT_0x4c4c38
// Used for seeking within asset files for parsing headers/data
DWORD adapter_SetFilePointer(
    HANDLE hFile,
    LONG lDistanceToMove,
    PLONG lpDistanceToMoveHigh,
    DWORD dwMoveMethod)
{
    if (hFile == INVALID_HANDLE_VALUE) return INVALID_SET_FILE_POINTER;
    
    FILE_HANDLE_INTERNAL *handle = (FILE_HANDLE_INTERNAL*)hFile;
    if (!handle->file) return INVALID_SET_FILE_POINTER;
    
    int whence;
    switch (dwMoveMethod) {
        case FILE_BEGIN:
            whence = SEEK_SET;
            break;
        case FILE_CURRENT:
            whence = SEEK_CUR;
            break;
        case FILE_END:
            whence = SEEK_END;
            break;
        default:
            return INVALID_SET_FILE_POINTER;
    }
    
    long long distance = lDistanceToMove;
    if (lpDistanceToMoveHigh) {
        distance |= ((long long)*lpDistanceToMoveHigh) << 32;
    }
    
    if (fseek(handle->file, (long)distance, whence) != 0) {
        return INVALID_SET_FILE_POINTER;
    }
    
    long position = ftell(handle->file);
    if (lpDistanceToMoveHigh) {
        *lpDistanceToMoveHigh = (LONG)(position >> 32);
    }
    
    return (DWORD)(position & 0xFFFFFFFF);
}

DWORD adapter_GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
    if (hFile == INVALID_HANDLE_VALUE) return INVALID_FILE_SIZE;
    
    FILE_HANDLE_INTERNAL *handle = (FILE_HANDLE_INTERNAL*)hFile;
    if (!handle->file) return INVALID_FILE_SIZE;
    
    struct stat st;
    if (stat(handle->path, &st) != 0) {
        return INVALID_FILE_SIZE;
    }
    
    if (lpFileSizeHigh) {
        *lpFileSizeHigh = (DWORD)(st.st_size >> 32);
    }
    
    return (DWORD)(st.st_size & 0xFFFFFFFF);
}

BOOL adapter_CloseHandle(HANDLE hObject)
{
    if (hObject == INVALID_HANDLE_VALUE) return FALSE;
    
    FILE_HANDLE_INTERNAL *handle = (FILE_HANDLE_INTERNAL*)hObject;
    if (handle->file) {
        fclose(handle->file);
    }
    if (handle->path) {
        free(handle->path);
    }
    free(handle);
    
    return TRUE;
}

/*
 * [RESOURCE] FindFirstFileA Adapter
 * PROV: va=0x402250, api=FindFirstFileA, xref_count=4, strings=["Sdata/*", "*.bmp", "*.wav", "*.dll"]
 * RESOURCE: Enumerates 277 files in Sdata/ directory
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Maps Win32 FindFirstFileA to POSIX opendir/readdir
 */
// PROV: FindFirstFileA/FindNextFileA for directory scanning
// Used to enumerate assets in Sdata/ directory
HANDLE adapter_FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
    if (!lpFileName || !lpFindFileData) return INVALID_HANDLE_VALUE;
    
    FIND_HANDLE_INTERNAL *find_handle = calloc(1, sizeof(FIND_HANDLE_INTERNAL));
    if (!find_handle) return INVALID_HANDLE_VALUE;
    
    // Extract directory from pattern
    char *dir_path = strdup(lpFileName);
    adapter_normalize_path(dir_path);
    
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        find_handle->pattern = strdup(last_slash + 1);
    } else {
        free(dir_path);
        dir_path = strdup(".");
        find_handle->pattern = strdup(lpFileName);
    }
    
#ifdef RESOURCE_WARNINGS
    // Check if scanning Sdata directory
    if (strstr(lpFileName, "Sdata") || strstr(lpFileName, "sdata")) {
        DIR* test = opendir(dir_path);
        if (!test) {
            fprintf(stderr, "[RESWARN] FindFirstFileA: Cannot access directory: %s\n", dir_path);
        } else {
            closedir(test);
        }
    }
#endif
    find_handle->dir = opendir(dir_path);
    free(dir_path);
    
    if (!find_handle->dir) {
        free(find_handle->pattern);
        free(find_handle);
        return INVALID_HANDLE_VALUE;
    }
    
    // Find first matching file
    if (!adapter_FindNextFileA((HANDLE)find_handle, lpFindFileData)) {
        adapter_FindClose((HANDLE)find_handle);
        return INVALID_HANDLE_VALUE;
    }
    
    return (HANDLE)find_handle;
}

BOOL adapter_FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
    if (hFindFile == INVALID_HANDLE_VALUE || !lpFindFileData) return FALSE;
    
    FIND_HANDLE_INTERNAL *find_handle = (FIND_HANDLE_INTERNAL*)hFindFile;
    if (!find_handle->dir) return FALSE;
    
    struct dirent *entry;
    while ((entry = readdir(find_handle->dir)) != NULL) {
        // Simple wildcard matching (only * supported for now)
        if (strcmp(find_handle->pattern, "*") == 0 || 
            strcmp(find_handle->pattern, "*.*") == 0 ||
            strstr(entry->d_name, find_handle->pattern) != NULL) {
            
            memset(lpFindFileData, 0, sizeof(WIN32_FIND_DATAA));
            strncpy(lpFindFileData->cFileName, entry->d_name, MAX_PATH - 1);
            
            // Get file attributes
            struct stat st;
            if (stat(entry->d_name, &st) == 0) {
                lpFindFileData->nFileSizeLow = (DWORD)(st.st_size & 0xFFFFFFFF);
                lpFindFileData->nFileSizeHigh = (DWORD)(st.st_size >> 32);
                
                lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
                if (S_ISDIR(st.st_mode)) {
                    lpFindFileData->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
                }
            }
            
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL adapter_FindClose(HANDLE hFindFile)
{
    if (hFindFile == INVALID_HANDLE_VALUE) return FALSE;
    
    FIND_HANDLE_INTERNAL *find_handle = (FIND_HANDLE_INTERNAL*)hFindFile;
    
    if (find_handle->dir) {
        closedir(find_handle->dir);
    }
    if (find_handle->pattern) {
        free(find_handle->pattern);
    }
    free(find_handle);
    
    return TRUE;
}

/*
 * [RESOURCE] LoadLibraryA Adapter
 * PROV: va=0x402000, api=LoadLibraryA, xref_count=5, strings=["data00.dll", "data31.dll", "Sdata/"]
 * RESOURCE: data00.dll through data31.dll (34 DLL archives) sha256=various kind=dll
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Maps Win32 LoadLibraryA to POSIX dlopen for DLL archives
 * WARN: DLL archives may contain packed resources - extraction required
 */
// PROV: LoadLibraryA call sites for data00.dll-data31.dll loading
// 32 DLL archives containing game data
HMODULE adapter_LoadLibraryA(LPCSTR lpLibFileName)
{
    if (!lpLibFileName) return NULL;
    
    char *normalized_path = strdup(lpLibFileName);
    adapter_normalize_path(normalized_path);
    
#ifdef RESOURCE_WARNINGS
    // Check for missing DLL archives
    if (strstr(normalized_path, "data") && strstr(normalized_path, ".dll")) {
        struct stat st;
        if (stat(normalized_path, &st) != 0) {
            fprintf(stderr, "[RESWARN] LoadLibraryA: Missing DLL archive: %s\n", normalized_path);
            #warning RESOURCE MISSING: DLL archives from Sdata/ may be missing
        }
    }
#endif
    void *handle = dlopen(normalized_path, RTLD_LAZY);
    free(normalized_path);
    
    return (HMODULE)handle;
}

/*
 * [RESOURCE] GetProcAddress Adapter
 * PROV: va=0x402050, api=GetProcAddress, xref_count=3, strings=["resource extraction"]
 * RESOURCE: Extracts packed resources from DLL archives
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Maps Win32 GetProcAddress to POSIX dlsym
 */
// PROV: GetProcAddress for DLL function/resource lookups
FARPROC adapter_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    if (!hModule || !lpProcName) return NULL;
    
    return (FARPROC)dlsym((void*)hModule, lpProcName);
}

// PROV: FreeLibrary for DLL cleanup
BOOL adapter_FreeLibrary(HMODULE hLibModule)
{
    if (!hLibModule) return FALSE;
    
    return (dlclose((void*)hLibModule) == 0);
}

// File dialog stubs (return failure - not commonly used in games)
BOOL adapter_GetOpenFileNameA(LPOPENFILENAMEA lpofn)
{
    return FALSE; // Not implemented - would require GUI toolkit
}

BOOL adapter_GetSaveFileNameA(LPOPENFILENAMEA lpofn)
{
    return FALSE; // Not implemented - would require GUI toolkit  
}

// Path utilities
DWORD adapter_GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer)
{
    if (!lpBuffer || nBufferLength == 0) return 0;
    
    if (getcwd(lpBuffer, nBufferLength) != NULL) {
        adapter_normalize_path(lpBuffer);
        return strlen(lpBuffer);
    }
    
    return 0;
}

BOOL adapter_SetCurrentDirectoryA(LPCSTR lpPathName)
{
    if (!lpPathName) return FALSE;
    
    char *normalized_path = strdup(lpPathName);
    adapter_normalize_path(normalized_path);
    
    int result = chdir(normalized_path);
    free(normalized_path);
    
    return (result == 0);
}

DWORD adapter_GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
    if (!lpFilename || nSize == 0) return 0;
    
    // Return executable path (simplified)
    ssize_t len = readlink("/proc/self/exe", lpFilename, nSize - 1);
    if (len != -1) {
        lpFilename[len] = '\0';
        adapter_normalize_path(lpFilename);
        return len;
    }
    
    return 0;
}

// Utility functions
void adapter_normalize_path(char *path)
{
    if (!path) return;
    
    // Convert Windows backslashes to forward slashes
    for (char *p = path; *p; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }
}

int adapter_file_exists(const char *path)
{
    if (!path) return 0;
    
    struct stat st;
    return (stat(path, &st) == 0);
}

int adapter_fs_init(void)
{
    return 0; // No initialization needed for POSIX
}

void adapter_fs_shutdown(void)
{
    // No cleanup needed for POSIX
}