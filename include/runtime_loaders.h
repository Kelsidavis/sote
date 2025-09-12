#ifndef RUNTIME_LOADERS_H
#define RUNTIME_LOADERS_H

#include <stdint.h>
#include "loader_structures.h"

/* 
 * SOTE Runtime Loaders - Stage 6 Implementation
 * Every function has evidence provenance from binary analysis
 * No speculative code - stubs where evidence is missing
 */

/* ---- Provenance Macros ------------------------------------------------------ */
#define PROV(msg)   /* PROV: msg */
#define NOTE(msg)   /* NOTE: msg */
#define TODO_EVID(msg) /* TODO: EVIDENCE REQUIRED — msg */

/* ---- Constants from Evidence ------------------------------------------------ */
// PROV: resource.catalog.json shows 43 BMPs all Windows 3.x format, 640x480x24 or 256x256x24
#define BMP_SIGNATURE     0x4D42  // PROV: VA_0x4dba3b constant 'BM'
#define BMP_HEADER_SIZE   54      // PROV: resource.catalog.json "bits offset 54" for all BMPs
#define BMP_INFO_SIZE     40      // PROV: Windows 3.x BITMAPINFOHEADER standard size

// PROV: resource.catalog.json shows 183 WAVs all PCM 16-bit mono
#define WAV_RIFF_MAGIC    0x46464952  // PROV: 'RIFF' little-endian
#define WAV_WAVE_MAGIC    0x45564157  // PROV: 'WAVE' little-endian
#define WAV_FMT_CHUNK     0x20746D66  // PROV: 'fmt ' little-endian
#define WAV_DATA_CHUNK    0x61746164  // PROV: 'data' little-endian

// PROV: bootpath.manifest.json mentions 32 DLL archives (data00.dll-data31.dll)
#define DLL_COUNT         32
#define PE_DOS_SIGNATURE  0x5A4D  // PROV: Standard PE 'MZ' signature

/* ---- File I/O Wrapper Functions --------------------------------------------- */

// PROV: VA_0x402100_file_system_setup uses CreateFileA/FindFirstFileA/GetCurrentDirectoryA
// PROV: runtime.apis.json shows CreateFileA at IAT 0x4c4634
FILE_HANDLE* file_open(const char* filename);

// PROV: VA_0x402200_asset_catalog_builder uses ReadFile/SetFilePointer  
// PROV: runtime.apis.json shows ReadFile usage for asset loading
int file_read(FILE_HANDLE* handle, void* buffer, uint32_t size);

// PROV: SetFilePointer usage in multiple loader functions
int file_seek(FILE_HANDLE* handle, uint32_t offset, int origin);

// PROV: CloseHandle calls after file operations
void file_close(FILE_HANDLE* handle);

// PROV: GetFileSize usage for buffer allocation
uint32_t file_get_size(FILE_HANDLE* handle);

/* ---- BMP Loader Functions --------------------------------------------------- */

// PROV: VA_0x403000_bmp_format_parser priority 69 in bootpath.manifest.json
// PROV: Uses CreateFileA/ReadFile/HeapAlloc per bootpath evidence
typedef struct {
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    uint8_t* pixelData;
    uint32_t pixelDataSize;
} BMP_IMAGE;

// PROV: Main BMP parsing function referenced at VA_0x403000
BMP_IMAGE* bmp_load(const char* filename);

// PROV: BMP header validation - checks magic, size constraints
int bmp_validate_header(BITMAPFILEHEADER* fileHeader, BITMAPINFOHEADER* infoHeader);

// PROV: Free BMP resources using HeapFree
void bmp_free(BMP_IMAGE* image);

/* ---- WAV Loader Functions --------------------------------------------------- */

// PROV: VA_0x403100_wav_format_parser priority 68 in bootpath.manifest.json
// PROV: Uses CreateFileA/ReadFile/HeapAlloc per bootpath evidence
typedef struct {
    RIFF_HEADER riffHeader;
    WAVE_FORMAT_CHUNK formatChunk;
    WAVE_DATA_CHUNK dataChunk;
    uint8_t* audioData;
    uint32_t audioDataSize;
} WAV_SOUND;

// PROV: Main WAV parsing function referenced at VA_0x403100
WAV_SOUND* wav_load(const char* filename);

// PROV: WAV header validation - checks RIFF/WAVE magic
int wav_validate_header(RIFF_HEADER* riffHeader);

// PROV: Free WAV resources using HeapFree
void wav_free(WAV_SOUND* sound);

/* ---- DLL Archive Loader Functions ------------------------------------------- */

// PROV: VA_0x402000_dll_data_archive_loader priority 79 in bootpath.manifest.json
// PROV: Uses LoadLibraryA/GetProcAddress/FreeLibrary per bootpath evidence
typedef struct {
    DLL_LOADER_CONTEXT contexts[DLL_COUNT];
    uint32_t loadedCount;
} DLL_ARCHIVE_MANAGER;

// PROV: Initialize DLL archive manager
DLL_ARCHIVE_MANAGER* dll_archive_init(void);

// PROV: Load a single DLL archive (data00.dll through data31.dll)
int dll_archive_load(DLL_ARCHIVE_MANAGER* manager, int index);

// PROV: Get resource from loaded DLL
void* dll_archive_get_resource(DLL_ARCHIVE_MANAGER* manager, int index, const char* name);

// PROV: Free all loaded DLLs
void dll_archive_cleanup(DLL_ARCHIVE_MANAGER* manager);

/* ---- SAN Movie Loader (Stub) ------------------------------------------------ */

// PROV: VA_0x403200_san_movie_parser priority 67 but evidence_strength "low"
// NOTE: Format unknown - needs reverse engineering
typedef struct {
    SAN_MOVIE_HEADER header;
    void* frameData;  // TODO_EVID: Unknown frame structure
} SAN_MOVIE;

// PROV: Stub for SAN loading - format needs analysis
SAN_MOVIE* san_load(const char* filename);

/* ---- Memory Management Wrappers --------------------------------------------- */

// PROV: VA_0x43d2aa_memory_allocator uses HeapAlloc/HeapReAlloc/HeapFree
// PROV: Implementation status "implemented" in bootpath.manifest.json
void* runtime_alloc(uint32_t size);
void* runtime_realloc(void* ptr, uint32_t size);
void runtime_free(void* ptr);

/* ---- Asset Catalog Builder -------------------------------------------------- */

// PROV: VA_0x402200_asset_catalog_builder builds catalog of 277 assets
// PROV: 43 BMP, 183 WAV, 17 SAN, 34 DLL per resource.catalog.json
typedef struct {
    char filename[256];
    uint32_t size;
    uint32_t type;  // 0=BMP, 1=WAV, 2=SAN, 3=DLL
    void* data;     // Cached data pointer
} ASSET_ENTRY;

typedef struct {
    ASSET_ENTRY* entries;
    uint32_t count;
    uint32_t capacity;
} ASSET_CATALOG;

// PROV: Build asset catalog from Sdata directory
ASSET_CATALOG* asset_catalog_build(const char* dataPath);

// PROV: Find asset by name
ASSET_ENTRY* asset_catalog_find(ASSET_CATALOG* catalog, const char* name);

// PROV: Free catalog and cached assets
void asset_catalog_free(ASSET_CATALOG* catalog);

#endif /* RUNTIME_LOADERS_H */