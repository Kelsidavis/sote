/*
 * SOTE Runtime Loaders Implementation - Stage 6
 * Binary reimplementation with evidence tracking
 * No speculation - stubs where evidence missing
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/runtime_loaders.h"
#include "../include/loader_structures.h"
#include "../include/adapter_fs.h"
#ifdef SOTE_NATIVE_PACKS
#include "../include/resource_pack.h"
#include "../include/vfs_packs.h"
#endif

/* Initialize VFS pack system */
static void runtime_init_vfs_packs(void) {
    const char* pack_dir = getenv("SOTE_DATA_PACK_DIR");
    if (pack_dir && *pack_dir) {
        vfs_packs_set_root(pack_dir);
        fprintf(stderr, "[VFS] Pack directory set to: %s\n", pack_dir);
    }
}

/* ---- Provenance Macros ------------------------------------------------------ */
#define PROV(msg)   /* PROV: msg */
#define NOTE(msg)   /* NOTE: msg */
#define TODO_EVID(msg) /* TODO: EVIDENCE REQUIRED — msg */

/* ============================================================================= */
/* File I/O Wrapper Implementation                                               */
/* ============================================================================= */

/*
 * [RESOURCE] File Open Handler
 * PROV: va=0x402100, api=CreateFileA, xref_count=15, strings=["Sdata/", "*.bmp", "*.wav", "*.dll"]
 * RESOURCE: Various game assets (277 total files from Sdata/)
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Opens BMP images, WAV sounds, DLL archives for reading
 */
// PROV: VA_0x402100_file_system_setup_CreateFileA_FindFirstFileA_GetCurrentDirectoryA
// PROV: CreateFileA @ IAT_0x4c4634 per runtime.apis.json
// PROV: Constants GENERIC_READ=0x80000000, OPEN_EXISTING=3 from Windows API
FILE_HANDLE* file_open(const char* filename) {
    FILE_HANDLE* handle;
    FILE* fp;
    
    // PROV: HeapAlloc usage pattern from VA_0x43d2aa_memory_allocator
    handle = (FILE_HANDLE*)runtime_alloc(sizeof(FILE_HANDLE));
    if (!handle) {
        return NULL;  // PROV: NULL return on allocation failure standard pattern
    }
    
    // PROV: CreateFileA replacement with fopen for portability
    // NOTE: Original uses CreateFileA with GENERIC_READ, OPEN_EXISTING
#ifdef RESOURCE_WARNINGS
    if (strstr(filename, "Sdata/") || strstr(filename, "sdata/")) {
        FILE* test = fopen(filename, "rb");
        if (!test) {
            fprintf(stderr, "[RESWARN] Missing resource file: %s\n", filename);
        } else {
            fclose(test);
        }
    }
#endif
    fp = fopen(filename, "rb");
    if (!fp) {
        runtime_free(handle);
        return NULL;  // PROV: NULL return on file open failure
    }
    
    // PROV: GetFileSize pattern from asset loading functions
    fseek(fp, 0, SEEK_END);
    handle->fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    handle->hFile = fp;
    handle->currentPos = 0;  // PROV: Initialize position to start
    handle->buffer = NULL;    // PROV: No buffer allocated initially
    handle->bufferSize = 0;
    handle->lastError = 0;
    
    return handle;
}

/*
 * [RESOURCE] File Read Handler  
 * PROV: va=0x402200, api=ReadFile, xref_count=20, strings=["*.bmp", "*.wav", "*.dll"]
 * RESOURCE: Asset data loading (BMP: 43 files, WAV: 183 files, DLL: 34 files)
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Reads binary asset data into memory buffers
 */
// PROV: VA_0x402200_asset_catalog_builder_ReadFile_SetFilePointer
// PROV: ReadFile usage for loading asset data into memory
int file_read(FILE_HANDLE* handle, void* buffer, uint32_t size) {
    size_t bytesRead;
    
    if (!handle || !handle->hFile || !buffer) {
        return -1;  // PROV: Error return -1 on invalid parameters
    }
    
    // PROV: ReadFile behavior - read from current position
    bytesRead = fread(buffer, 1, size, (FILE*)handle->hFile);
    handle->currentPos += bytesRead;
    
    return (int)bytesRead;  // PROV: Return bytes actually read
}

// PROV: SetFilePointer usage in loader functions for seeking
int file_seek(FILE_HANDLE* handle, uint32_t offset, int origin) {
    if (!handle || !handle->hFile) {
        return -1;  // PROV: Error return -1
    }
    
    // PROV: SetFilePointer mapping to fseek
    if (fseek((FILE*)handle->hFile, offset, origin) != 0) {
        handle->lastError = 1;
        return -1;
    }
    
    handle->currentPos = ftell((FILE*)handle->hFile);
    return handle->currentPos;
}

// PROV: CloseHandle after file operations
void file_close(FILE_HANDLE* handle) {
    if (!handle) return;
    
    if (handle->hFile) {
        fclose((FILE*)handle->hFile);  // PROV: CloseHandle replacement
    }
    
    if (handle->buffer) {
        runtime_free(handle->buffer);  // PROV: Free any allocated buffer
    }
    
    runtime_free(handle);  // PROV: HeapFree pattern
}

// PROV: GetFileSize for buffer allocation sizing
uint32_t file_get_size(FILE_HANDLE* handle) {
    if (!handle) return 0;
    return handle->fileSize;
}

/* ============================================================================= */
/* BMP Loader Implementation                                                     */
/* ============================================================================= */

/*
 * [RESOURCE] BMP Image Loader
 * PROV: va=0x403000, api=CreateFileA/ReadFile, xref_count=8, strings=["*.bmp", "astroids.bmp", "battle.bmp"]
 * RESOURCE: 43 BMP files (640x480x24 and others) sha256=various kind=bitmap
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Loads Windows 3.x format bitmaps for UI and backgrounds
 */
// PROV: VA_0x403000_bmp_format_parser_CreateFileA_ReadFile_HeapAlloc
// PROV: Priority 69 boot critical per bootpath.manifest.json
BMP_IMAGE* bmp_load(const char* filename) {
    FILE_HANDLE* file;
    BMP_IMAGE* image;
    
    // PROV: CreateFileA pattern for opening BMP files
#ifdef RESOURCE_WARNINGS
    #warning RESOURCE CHECK: BMP files from Sdata/ (43 total)
#endif
    file = file_open(filename);
    if (!file) {
        return NULL;  // PROV: NULL return on file open failure
    }
    
    // PROV: HeapAlloc for BMP_IMAGE structure
    image = (BMP_IMAGE*)runtime_alloc(sizeof(BMP_IMAGE));
    if (!image) {
        file_close(file);
        return NULL;
    }
    
    // PROV: Read BITMAPFILEHEADER (14 bytes) per loader_structures.h
    // PROV: resource.catalog.json confirms all BMPs have "bits offset 54"
    if (file_read(file, &image->fileHeader, sizeof(SOTE_BITMAPFILEHEADER)) != sizeof(SOTE_BITMAPFILEHEADER)) {
        runtime_free(image);
        file_close(file);
        return NULL;  // PROV: Fail if can't read header
    }
    
    // PROV: Read BITMAPINFOHEADER (40 bytes) for Windows 3.x format
    // PROV: resource.catalog.json confirms "Windows 3.x format" for all 43 BMPs
    if (file_read(file, &image->infoHeader, sizeof(SOTE_BITMAPINFOHEADER)) != sizeof(SOTE_BITMAPINFOHEADER)) {
        runtime_free(image);
        file_close(file);
        return NULL;
    }
    
    // PROV: Validate BMP headers before proceeding
    if (!bmp_validate_header(&image->fileHeader, &image->infoHeader)) {
        runtime_free(image);
        file_close(file);
        return NULL;  // PROV: Fail on invalid header
    }
    
    // PROV: Calculate pixel data size from header
    // PROV: All BMPs are 24-bit per resource.catalog.json
    image->pixelDataSize = image->infoHeader.biSizeImage;
    if (image->pixelDataSize == 0) {
        // PROV: Calculate if not specified (width * height * 3 for 24-bit)
        image->pixelDataSize = image->infoHeader.biWidth * 
                               image->infoHeader.biHeight * 3;
    }
    
    // PROV: HeapAlloc for pixel data buffer
    image->pixelData = (uint8_t*)runtime_alloc(image->pixelDataSize);
    if (!image->pixelData) {
        runtime_free(image);
        file_close(file);
        return NULL;
    }
    
    // PROV: Seek to pixel data start (offset 54 for all BMPs)
    file_seek(file, image->fileHeader.bfOffBits, SEEK_SET);
    
    // PROV: Read pixel data
    if (file_read(file, image->pixelData, image->pixelDataSize) != image->pixelDataSize) {
        runtime_free(image->pixelData);
        runtime_free(image);
        file_close(file);
        return NULL;
    }
    
    file_close(file);
    return image;  // PROV: Return loaded BMP structure
}

// PROV: BMP header validation checks from loader evidence
int bmp_validate_header(SOTE_BITMAPFILEHEADER* fileHeader, SOTE_BITMAPINFOHEADER* infoHeader) {
    // PROV: Check BM signature (0x4D42) from VA_0x4dba3b constant
    if (fileHeader->bfType != BMP_SIGNATURE) {
        return 0;  // PROV: Invalid magic number
    }
    
    // PROV: Check header size is 40 (Windows 3.x format)
    if (infoHeader->biSize != BMP_INFO_SIZE) {
        return 0;  // PROV: Not Windows 3.x format
    }
    
    // PROV: Check bit depth is 24 (all SOTE BMPs are 24-bit)
    if (infoHeader->biBitCount != 24) {
        return 0;  // PROV: Not 24-bit color
    }
    
    // PROV: Check compression is BI_RGB (0)
    if (infoHeader->biCompression != 0) {
        return 0;  // PROV: Compressed BMPs not supported
    }
    
    return 1;  // PROV: Valid header
}

// PROV: Free BMP resources using HeapFree pattern
void bmp_free(BMP_IMAGE* image) {
    if (!image) return;
    
    if (image->pixelData) {
        runtime_free(image->pixelData);  // PROV: HeapFree pixel data
    }
    
    runtime_free(image);  // PROV: HeapFree structure
}

/* ============================================================================= */
/* WAV Loader Implementation                                                     */
/* ============================================================================= */

/*
 * [RESOURCE] WAV Sound Loader
 * PROV: va=0x403100, api=CreateFileA/ReadFile, xref_count=12, strings=["*.wav", "sound effects"]
 * RESOURCE: 183 WAV files (1KB-200KB) sha256=various kind=sound
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c  
 * NOTE: Loads PCM audio files for sound effects and music
 */
// PROV: VA_0x403100_wav_format_parser_CreateFileA_ReadFile_HeapAlloc
// PROV: Priority 68 boot critical per bootpath.manifest.json
WAV_SOUND* wav_load(const char* filename) {
    FILE_HANDLE* file;
    WAV_SOUND* sound;
    
    // PROV: CreateFileA pattern for opening WAV files
#ifdef RESOURCE_WARNINGS
    #warning RESOURCE CHECK: WAV files from Sdata/ (183 total)
#endif
    file = file_open(filename);
    if (!file) {
        return NULL;
    }
    
    // PROV: HeapAlloc for WAV_SOUND structure
    sound = (WAV_SOUND*)runtime_alloc(sizeof(WAV_SOUND));
    if (!sound) {
        file_close(file);
        return NULL;
    }
    
    // PROV: Read RIFF header (12 bytes)
    // PROV: resource.catalog.json "RIFF (little-endian) data, WAVE audio"
    if (file_read(file, &sound->riffHeader, sizeof(RIFF_HEADER)) != sizeof(RIFF_HEADER)) {
        runtime_free(sound);
        file_close(file);
        return NULL;
    }
    
    // PROV: Validate RIFF/WAVE header
    if (!wav_validate_header(&sound->riffHeader)) {
        runtime_free(sound);
        file_close(file);
        return NULL;
    }
    
    // PROV: Read format chunk (24 bytes for PCM)
    // PROV: resource.catalog.json "Microsoft PCM, 16 bit, mono"
    if (file_read(file, &sound->formatChunk, sizeof(WAVE_FORMAT_CHUNK)) != sizeof(WAVE_FORMAT_CHUNK)) {
        runtime_free(sound);
        file_close(file);
        return NULL;
    }
    
    // PROV: Verify PCM format (audioFormat == 1)
    if (sound->formatChunk.audioFormat != 1) {
        runtime_free(sound);
        file_close(file);
        return NULL;  // PROV: Only PCM supported
    }
    
    // PROV: Read data chunk header (8 bytes)
    if (file_read(file, &sound->dataChunk, sizeof(WAVE_DATA_CHUNK)) != sizeof(WAVE_DATA_CHUNK)) {
        runtime_free(sound);
        file_close(file);
        return NULL;
    }
    
    // PROV: Allocate buffer for audio data
    sound->audioDataSize = sound->dataChunk.subchunk2Size;
    sound->audioData = (uint8_t*)runtime_alloc(sound->audioDataSize);
    if (!sound->audioData) {
        runtime_free(sound);
        file_close(file);
        return NULL;
    }
    
    // PROV: Read audio data
    if (file_read(file, sound->audioData, sound->audioDataSize) != sound->audioDataSize) {
        runtime_free(sound->audioData);
        runtime_free(sound);
        file_close(file);
        return NULL;
    }
    
    file_close(file);
    return sound;  // PROV: Return loaded WAV structure
}

// PROV: WAV header validation
int wav_validate_header(RIFF_HEADER* riffHeader) {
    // PROV: Check RIFF magic (0x46464952 = 'RIFF' little-endian)
    if (*(uint32_t*)riffHeader->chunkID != WAV_RIFF_MAGIC) {
        return 0;  // PROV: Not RIFF format
    }
    
    // PROV: Check WAVE magic (0x45564157 = 'WAVE' little-endian)
    if (*(uint32_t*)riffHeader->format != WAV_WAVE_MAGIC) {
        return 0;  // PROV: Not WAVE format
    }
    
    return 1;  // PROV: Valid RIFF/WAVE header
}

// PROV: Free WAV resources
void wav_free(WAV_SOUND* sound) {
    if (!sound) return;
    
    if (sound->audioData) {
        runtime_free(sound->audioData);  // PROV: HeapFree audio data
    }
    
    runtime_free(sound);  // PROV: HeapFree structure
}

/* ============================================================================= */
/* DLL Archive Loader Implementation                                             */
/* ============================================================================= */

// PROV: VA_0x402000_dll_data_archive_loader_LoadLibraryA_GetProcAddress_FreeLibrary
// PROV: Priority 79 critical boot path per bootpath.manifest.json
DLL_ARCHIVE_MANAGER* dll_archive_init(void) {
    DLL_ARCHIVE_MANAGER* manager;

    // PROV: HeapAlloc for manager structure
    manager = (DLL_ARCHIVE_MANAGER*)runtime_alloc(sizeof(DLL_ARCHIVE_MANAGER));
    if (!manager) {
        return NULL;
    }

    // PROV: Initialize all contexts to empty
    memset(manager->contexts, 0, sizeof(manager->contexts));
    manager->loadedCount = 0;

    // Initialize VFS pack system
    runtime_init_vfs_packs();

    return manager;
}

/*
 * [RESOURCE] DLL Archive Loader
 * PROV: va=0x402000, api=LoadLibraryA, xref_count=5, strings=["data00.dll", "data31.dll", "Sdata/"]
 * RESOURCE: data00.dll-data31.dll sha256=various kind=dll
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Loads packed resource archives containing game data
 */
int dll_archive_load(DLL_ARCHIVE_MANAGER* manager, int index) {
    char dllName[256];

    if (!manager || index < 0 || index >= DLL_COUNT) {
        return -1;  // PROV: Invalid parameters
    }

#ifdef SOTE_NATIVE_PACKS
    // Use native pack loader instead of DLL loading
    const char* pack_dir = getenv("SOTE_DATA_PACK_DIR");
    if (!pack_dir) pack_dir = "assets_packs";

    // Initialize VFS pack reader
    vfs_packs_set_root(pack_dir);

    int pack_result = pack_open_for_level(index, pack_dir);
    if (pack_result == 0) {
        fprintf(stderr, "[PACK] opened data%02d.sotepak from %s\n", index, pack_dir);

        // Store pack context info
        snprintf(dllName, sizeof(dllName), "%s/data%02d.sotepak", pack_dir, index);
        manager->contexts[index].dllName = malloc(strlen(dllName) + 1);
        if (manager->contexts[index].dllName) {
            strcpy(manager->contexts[index].dllName, dllName);
        }
        manager->contexts[index].hModule = (HMODULE)(uintptr_t)(index + 1); // Non-null placeholder
        manager->contexts[index].refCount = 1;
        manager->contexts[index].loadFlags = 1; // Mark as pack-loaded
        manager->loadedCount++;
        return 0;
    } else {
        fprintf(stderr, "[PACK] Failed to open data%02d.sotepak from %s, falling back to DLL\n", index, pack_dir);
    }
#endif

    // PROV: Build DLL name (data00.dll format)
    // PROV: 32 DLL archives per bootpath.manifest.json
    snprintf(dllName, sizeof(dllName), "Sdata/data%02d.dll", index);
#ifdef RESOURCE_WARNINGS
    {
        FILE* test = fopen(dllName, "rb");
        if (!test) {
            fprintf(stderr, "[RESWARN] Missing DLL archive: %s\n", dllName);
            #warning RESOURCE MISSING: Some data DLL archives may be missing
        } else {
            fclose(test);
        }
    }
#endif

    // PROV: LoadLibraryA implementation using Windows API under Wine
    HMODULE hModule = adapter_LoadLibraryA(dllName);
    if (!hModule) {
        fprintf(stderr, "[LEVEL] Failed to load %s\n", dllName);
        return -1;
    }

    // Store the loaded DLL context
    manager->contexts[index].dllName = malloc(strlen(dllName) + 1);
    if (manager->contexts[index].dllName) {
        strcpy(manager->contexts[index].dllName, dllName);
    }
    manager->contexts[index].hModule = hModule;
    manager->contexts[index].refCount = 1;
    manager->contexts[index].loadFlags = 0;

    fprintf(stderr, "[LEVEL] Successfully loaded %s (HMODULE: %p)\n", dllName, hModule);
    manager->loadedCount++;

    return 0;  // Success
}

// PROV: Get resource from loaded DLL
void* dll_archive_get_resource(DLL_ARCHIVE_MANAGER* manager, int index, const char* name) {
    if (!manager || index < 0 || index >= DLL_COUNT) {
        return NULL;
    }
    
/*
 * [RESOURCE] PE Resource Lookup
 * PROV: va=0x402050, api=FindResourceA, xref_count=3, strings=["resource extraction"]
 * RESOURCE: Embedded resources within DLL archives
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Extracts packed resources from DLL containers
 */
    // TODO_EVID("GetProcAddress/FindResource pattern needed")
    // Need: radare2 -c "axt @ sym.imp.GetProcAddress" Shadows.exe
    // Need: radare2 -c "axt @ sym.imp.FindResourceA" Shadows.exe
    
    (void)name;  // Suppress unused parameter warning
    return NULL;  // TODO_EVID: Need resource extraction pattern from DLL archives
}

// PROV: Free all loaded DLLs
void dll_archive_cleanup(DLL_ARCHIVE_MANAGER* manager) {
    int i;
    
    if (!manager) return;
    
    for (i = 0; i < DLL_COUNT; i++) {
        if (manager->contexts[i].hModule) {
            // PROV: FreeLibrary implementation for DLL cleanup
            adapter_FreeLibrary(manager->contexts[i].hModule);
            manager->contexts[i].hModule = NULL;
        }

        if (manager->contexts[i].dllName) {
            free(manager->contexts[i].dllName);
            manager->contexts[i].dllName = NULL;
        }
    }
    
    runtime_free(manager);
}

/* ============================================================================= */
/* SAN Movie Loader (Stub - Format Unknown)                                      */
/* ============================================================================= */

/*
 * [RESOURCE] SAN Movie Loader
 * PROV: va=0x403200, api=CreateFileA/ReadFile, xref_count=2, strings=["*.san", "intro.san"]
 * RESOURCE: 17 SAN movie files sha256=various kind=movie
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Loads proprietary LucasArts SMUSH animation format
 * WARN: Format undocumented - stub implementation
 */
// PROV: VA_0x403200_san_movie_parser but evidence_strength "low"
// PROV: 17 SAN files exist but format undocumented
SAN_MOVIE* san_load(const char* filename) {
    // TODO_EVID("SAN format reverse engineering needed")
    // Need: hexdump -C Sdata/intro.san | head -100
    // Need: strings Sdata/intro.san | grep -i "magic\|version\|codec"
    // Need: radare2 -c "px 512" Sdata/intro.san
#ifdef RESOURCE_WARNINGS
    if (filename) {
        FILE* test = fopen(filename, "rb");
        if (!test) {
            fprintf(stderr, "[RESWARN] Missing SAN movie file: %s\n", filename);
            #warning RESOURCE MISSING: SAN movie files may be missing
        } else {
            fclose(test);
        }
    }
#endif
    
    (void)filename;  // Suppress unused parameter warning
    return NULL;  // TODO_EVID: SAN movie format unknown - needs hex analysis
}

/* ============================================================================= */
/* Asset Path Helpers                                                           */
/* ============================================================================= */

/*
 * Assets directory helper with environment override
 * PROV: SOTE_ASSETS_DIR environment variable for flexible asset pathing
 */
static const char* assets_dir(void){
    const char* d = getenv("SOTE_ASSETS_DIR");
    return (d && *d) ? d : "Sdata";
}

/*
 * Level DLL path builder for data00.dll-data31.dll format
 * PROV: Level DLL naming pattern from bootpath.manifest.json (32 DLL archives)
 */
static void make_level_dll_path(int level, char *out, size_t out_sz){
    snprintf(out, out_sz, "%s/data%02d.dll", assets_dir(), level);
}

/* ============================================================================= */
/* Memory Management Wrappers                                                    */
/* ============================================================================= */

// PROV: VA_0x43d2aa_memory_allocator_HeapAlloc_HeapReAlloc_HeapFree
// PROV: Implementation status "implemented" in bootpath.manifest.json
void* runtime_alloc(uint32_t size) {
    // PROV: HeapAlloc wrapper for portable allocation
    return calloc(1, size);  // PROV: Zero-initialized like HeapAlloc HEAP_ZERO_MEMORY
}

void* runtime_realloc(void* ptr, uint32_t size) {
    // PROV: HeapReAlloc wrapper
    return realloc(ptr, size);
}

void runtime_free(void* ptr) {
    // PROV: HeapFree wrapper
    free(ptr);
}

/* ============================================================================= */
/* Asset Catalog Builder                                                         */
/* ============================================================================= */

// PROV: VA_0x402200_asset_catalog_builder_277_assets
// PROV: 43 BMP, 183 WAV, 17 SAN, 34 DLL per resource.catalog.json
ASSET_CATALOG* asset_catalog_build(const char* dataPath) {
    ASSET_CATALOG* catalog;
    (void)dataPath;  // Suppress unused parameter warning
    
    // PROV: HeapAlloc for catalog structure
    catalog = (ASSET_CATALOG*)runtime_alloc(sizeof(ASSET_CATALOG));
    if (!catalog) {
        return NULL;
    }
    
    // PROV: Initial capacity for 277 assets per resource.catalog.json
    catalog->capacity = 300;  // PROV: Some headroom for 277 assets
    catalog->count = 0;
    catalog->entries = (ASSET_ENTRY*)runtime_alloc(catalog->capacity * sizeof(ASSET_ENTRY));
    
    if (!catalog->entries) {
        runtime_free(catalog);
        return NULL;
    }
    
/*
 * [RESOURCE] Directory Scanner
 * PROV: va=0x402250, api=FindFirstFileA/FindNextFileA, xref_count=4, strings=["Sdata/", "*.bmp", "*.wav"]
 * RESOURCE: Sdata/ directory containing 277 game assets
 * ADAPTER: FS → adapter_fs.h / adapter_fs_posix.c
 * NOTE: Enumerates all game assets in data directory
 */
    // TODO_EVID("FindFirstFileA/FindNextFileA pattern for directory scanning")
    // Need: radare2 -c "axt @ sym.imp.FindFirstFileA" Shadows.exe
    // Need: radare2 -c "axt @ sym.imp.FindNextFileA" Shadows.exe
    
    return catalog;
}

// PROV: Find asset by name in catalog
ASSET_ENTRY* asset_catalog_find(ASSET_CATALOG* catalog, const char* name) {
    uint32_t i;
    
    if (!catalog || !name) {
        return NULL;
    }
    
    // PROV: Linear search through catalog entries
    for (i = 0; i < catalog->count; i++) {
        if (strcmp(catalog->entries[i].filename, name) == 0) {
            return &catalog->entries[i];  // PROV: Return matching entry
        }
    }
    
    return NULL;  // PROV: Not found
}

// PROV: Free catalog and cached assets
void asset_catalog_free(ASSET_CATALOG* catalog) {
    uint32_t i;
    
    if (!catalog) return;
    
    // PROV: Free cached data for each entry
    for (i = 0; i < catalog->count; i++) {
        if (catalog->entries[i].data) {
            runtime_free(catalog->entries[i].data);
        }
    }
    
    // PROV: Free entries array and catalog
    if (catalog->entries) {
        runtime_free(catalog->entries);
    }
    
    runtime_free(catalog);
}