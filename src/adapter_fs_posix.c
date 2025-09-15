#ifdef LINUX_BUILD
#define _GNU_SOURCE
#endif

#include "adapter_fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef _WIN32
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dlfcn.h>
#else
// Windows compatibility
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#define S_ISDIR(x) ((x) & _S_IFDIR)
#endif

// PROV: VFS trace file handle for logging all file access operations
static FILE* g_vfs_trace_file = NULL;

// Forward declarations for VFS functions
static void vfs_init_trace(void);
static void vfs_log_trace(const char* operation, const char* path, const char* result, const char* resolved_path);
static char* vfs_resolve_path(const char* relative_path);

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
#ifndef _WIN32
    DIR *dir;
    struct dirent *current;
#else
    HANDLE win_handle;
    WIN32_FIND_DATAA win_data;
    int first_call;
#endif
    char *pattern;
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
    
    // PROV: Log file open attempt to VFS trace during Title/Start phases
    vfs_log_trace("open", normalized_path, "attempt", NULL);
    
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
        // PROV: Log failed file open to VFS trace
        vfs_log_trace("open", normalized_path, "failed", NULL);
        free(normalized_path);
        free(handle);
        return INVALID_HANDLE_VALUE;
    }
    
    // PROV: Log successful file open to VFS trace
    vfs_log_trace("open", normalized_path, "success", normalized_path);
    
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
    
    // PROV: Log read operation to VFS trace with byte count
    if (handle->path) {
        char result_str[64];
        snprintf(result_str, sizeof(result_str), "read_%zu_bytes", bytes_read);
        vfs_log_trace("read", handle->path, result_str, NULL);
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
    
    // PROV: Log file close to VFS trace
    if (handle->path) {
        vfs_log_trace("close", handle->path, "success", NULL);
    }
    
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
    
#ifdef _WIN32
    // Use native Windows implementation
    return FindFirstFileA(lpFileName, lpFindFileData);
#else
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
#endif
}

BOOL adapter_FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
    if (hFindFile == INVALID_HANDLE_VALUE || !lpFindFileData) return FALSE;
    
#ifdef _WIN32
    // Use native Windows implementation
    return FindNextFileA(hFindFile, lpFindFileData);
#else
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
#endif
}

BOOL adapter_FindClose(HANDLE hFindFile)
{
    if (hFindFile == INVALID_HANDLE_VALUE) return FALSE;
    
#ifdef _WIN32
    // Use native Windows implementation
    return FindClose(hFindFile);
#else
    FIND_HANDLE_INTERNAL *find_handle = (FIND_HANDLE_INTERNAL*)hFindFile;
    
    if (find_handle->dir) {
        closedir(find_handle->dir);
    }
    if (find_handle->pattern) {
        free(find_handle->pattern);
    }
    free(find_handle);
    
    return TRUE;
#endif
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
#ifndef _WIN32
    void *handle = dlopen(normalized_path, RTLD_LAZY);
    free(normalized_path);
    return (HMODULE)handle;
#else
    // Windows LoadLibraryA implementation
    HMODULE handle = LoadLibraryA(normalized_path);
    free(normalized_path);
    return handle;
#endif
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
    
#ifndef _WIN32    
    return (FARPROC)dlsym((void*)hModule, lpProcName);
#else
    return GetProcAddress(hModule, lpProcName);
#endif
}

// PROV: FreeLibrary for DLL cleanup
BOOL adapter_FreeLibrary(HMODULE hLibModule)
{
    if (!hLibModule) return FALSE;
    
#ifndef _WIN32
    return (dlclose((void*)hLibModule) == 0);
#else
    return FreeLibrary(hLibModule);
#endif
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
    
#ifndef _WIN32
    if (getcwd(lpBuffer, nBufferLength) != NULL) {
#else
    if (_getcwd(lpBuffer, nBufferLength) != NULL) {
#endif
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
    
#ifndef _WIN32
    int result = chdir(normalized_path);
#else
    int result = _chdir(normalized_path);
#endif
    free(normalized_path);
    
    return (result == 0);
}

DWORD adapter_GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
    if (!lpFilename || nSize == 0) return 0;
    
    // Return executable path (simplified)
#ifndef _WIN32
    ssize_t len = readlink("/proc/self/exe", lpFilename, nSize - 1);
    if (len != -1) {
        lpFilename[len] = '\0';
        adapter_normalize_path(lpFilename);
        return len;
    }
#else
    // Windows implementation
    DWORD len = GetModuleFileNameA(hModule, lpFilename, nSize);
    if (len > 0 && len < nSize) {
        adapter_normalize_path(lpFilename);
        return len;
    }
#endif
    
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
    // PROV: Close VFS trace file if open
    if (g_vfs_trace_file) {
        vfs_log_trace("shutdown", "adapter_fs", "complete", NULL);
        fclose(g_vfs_trace_file);
        g_vfs_trace_file = NULL;
    }
    // No other cleanup needed for POSIX
}

/* RESOURCE_VFS_START */
/*
 * [RESOURCE] Minimal Resource VFS Implementation
 * PROV: resources.manifest.json - 277 files total (43 BMP, 183 WAV, 34 DLL, 17 SAN)
 * VFS: Deterministic search order for diskless runtime: env → ./Sdata → ../Sdata → ../../Sdata
 * ADAPTER: FS → adapter_fs_posix.c (POSIX VFS implementation)
 * NOTE: Evidence-based file resolution with logging for troubleshooting
 */

// PROV: Based on runtime/resources.manifest.json asset inventory
static const char* vfs_search_roots[] = {
    NULL,           // Will be filled with SOTE_ASSETS_DIR from environment
    "./Sdata",      // Current directory relative
    "../Sdata",     // Parent directory relative  
    "../../Sdata",  // Grandparent directory relative
    NULL
};

/*
 * [RESOURCE] Initialize VFS tracing to JSONL file
 * PROV: Output VFS trace to /media/k/vbox1/Shadows/SOTE/reports/runtime/vfs.start.trace.jsonl
 * NOTE: Creates directory if needed and opens trace file for append
 */
static void vfs_init_trace(void) {
    if (g_vfs_trace_file) return;  // Already initialized
    
    // PROV: Ensure reports/runtime directory exists
    #ifndef _WIN32
    system("mkdir -p /media/k/vbox1/Shadows/SOTE/reports/runtime");
    #else
    system("mkdir /media/k/vbox1/Shadows/SOTE/reports/runtime 2>nul");
    #endif
    
    g_vfs_trace_file = fopen("/media/k/vbox1/Shadows/SOTE/reports/runtime/vfs.start.trace.jsonl", "a");
    if (g_vfs_trace_file) {
        // PROV: Log trace file initialization
        fprintf(stderr, "[VFS] Trace file opened: /media/k/vbox1/Shadows/SOTE/reports/runtime/vfs.start.trace.jsonl\n");
    }
}

/*
 * [RESOURCE] Log VFS operation to trace file in JSONL format
 * PROV: JSON Lines format for easy parsing and analysis
 * NOTE: Logs timestamp, operation, path, result, and resolved path
 */
static void vfs_log_trace(const char* operation, const char* path, const char* result, const char* resolved_path) {
    vfs_init_trace();  // Ensure trace file is open
    
    if (!g_vfs_trace_file) return;
    
    // Get current timestamp
    time_t now = time(NULL);
    
    // Write JSONL entry
    fprintf(g_vfs_trace_file, "{\"timestamp\":%ld,\"operation\":\"%s\",\"path\":\"%s\",\"result\":\"%s\",\"resolved\":\"%s\"}\n",
            now, operation ? operation : "unknown", 
            path ? path : "null", 
            result ? result : "unknown",
            resolved_path ? resolved_path : "null");
    
    // Flush to ensure data is written immediately
    fflush(g_vfs_trace_file);
}

/*
 * [RESOURCE] VFS Path Resolution Function
 * PROV: resources.manifest.json - handles 277 game assets with deterministic search
 * SEARCH: env(SOTE_ASSETS_DIR) → ./Sdata → ../Sdata → ../../Sdata
 * LOGGING: [VFS] resolved for success, [WARN][VFS] missing for failures
 * NOTE: Returns allocated path string that must be freed by caller
 */
static char* vfs_resolve_path(const char* relative_path) {
    if (!relative_path) return NULL;
    
    // Initialize environment path if not already done
    static int env_initialized = 0;
    if (!env_initialized) {
        vfs_search_roots[0] = getenv("SOTE_ASSETS_DIR");
        env_initialized = 1;
    }
    
    // Try each search root in order
    for (int i = 0; vfs_search_roots[i] != NULL; i++) {
        const char* root = vfs_search_roots[i];
        if (!root) continue;
        
        // Construct full path
        size_t root_len = strlen(root);
        size_t rel_len = strlen(relative_path);
        char* full_path = malloc(root_len + rel_len + 2); // +2 for '/' and '\0'
        
        if (!full_path) continue;
        
        strcpy(full_path, root);
        if (root[root_len - 1] != '/') {
            strcat(full_path, "/");
        }
        strcat(full_path, relative_path);
        
        // Normalize path separators
        adapter_normalize_path(full_path);
        
        // Check if file exists
        if (adapter_file_exists(full_path)) {
            fprintf(stderr, "[VFS] resolved: %s -> %s\n", relative_path, full_path);
            // PROV: Log successful resolution to trace file
            vfs_log_trace("resolve", relative_path, "success", full_path);
            return full_path;
        }
        
        free(full_path);
    }
    
    // File not found in any search root
    fprintf(stderr, "[WARN][VFS] missing: %s (searched all VFS roots)\n", relative_path);
    // PROV: Log failed resolution to trace file
    vfs_log_trace("resolve", relative_path, "missing", NULL);
    return NULL;
}

/*
 * [RESOURCE] VFS-aware file open adapter
 * PROV: resources.manifest.json evidence for asset access patterns
 * ROUTE: All adapter_fs_open() calls through VFS resolution
 * LOGGING: [ERR][VFS] fopen failed for troubleshooting
 * NOTE: Routes to CreateFileA after VFS path resolution
 */
HANDLE adapter_fs_open(const char* path, DWORD access, DWORD creation) {
    if (!path) return INVALID_HANDLE_VALUE;
    
    char* resolved_path = vfs_resolve_path(path);
    if (!resolved_path) {
        // Fallback to original path for absolute paths or non-Sdata files
        resolved_path = strdup(path);
        if (!resolved_path) return INVALID_HANDLE_VALUE;
    }
    
    HANDLE result = adapter_CreateFileA(resolved_path, access, 0, NULL, creation, 
                                       FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (result == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "[ERR][VFS] fopen failed: %s\n", resolved_path);
    }
    
    free(resolved_path);
    return result;
}

/*
 * [RESOURCE] VFS-aware file existence check
 * PROV: resources.manifest.json asset verification
 * CHECK: Uses VFS resolution before existence test
 * LOGGING: Leverages vfs_resolve_path logging
 * NOTE: Returns 1 if file found in VFS, 0 otherwise
 */
int adapter_fs_exists(const char* path) {
    if (!path) return 0;
    
    char* resolved_path = vfs_resolve_path(path);
    if (!resolved_path) {
        // Fallback check for non-VFS paths
        return adapter_file_exists(path);
    }
    
    int exists = adapter_file_exists(resolved_path);
    free(resolved_path);
    return exists;
}