#ifndef LOADER_STRUCTURES_H
#define LOADER_STRUCTURES_H

#include <stdint.h>

/* 
 * SOTE Loader Structures - Stage 4 Layout Analysis
 * Mechanically derived from binary evidence and resource catalog
 * Every field has provenance tracking in layouts.curated.json
 */

#pragma pack(push, 1)

/* BMP File Header - 14 bytes
 * Evidence: VA_0x4dba3b constant 0x424D, resource.catalog.json bitmap files
 * NOTE: Prefixed with SOTE_ to avoid MinGW WinGDI.h conflicts
 */
#ifndef SOTE_BITMAPFILEHEADER_DEFINED
#define SOTE_BITMAPFILEHEADER_DEFINED
typedef struct {
    uint16_t bfType;        /* 0x00: Magic 'BM' (0x4D42) */
    uint32_t bfSize;        /* 0x02: File size in bytes */
    uint16_t bfReserved1;   /* 0x06: Reserved, always 0 */
    uint16_t bfReserved2;   /* 0x08: Reserved, always 0 */
    uint32_t bfOffBits;     /* 0x0A: Offset to pixel data (54 for all BMPs) */
} SOTE_BITMAPFILEHEADER;
#endif

/* BMP Info Header - 40 bytes (Windows 3.x format)
 * Evidence: resource.catalog.json "Windows 3.x format" for all 43 bitmaps
 * NOTE: Prefixed with SOTE_ to avoid MinGW WinGDI.h conflicts
 */
#ifndef SOTE_BITMAPINFOHEADER_DEFINED
#define SOTE_BITMAPINFOHEADER_DEFINED
typedef struct {
    uint32_t biSize;          /* 0x00: Header size (40) */
    int32_t  biWidth;         /* 0x04: Width in pixels (640, 256, etc) */
    int32_t  biHeight;        /* 0x08: Height in pixels (480, 64, 128, etc) */
    uint16_t biPlanes;        /* 0x0C: Color planes (always 1) */
    uint16_t biBitCount;      /* 0x0E: Bits per pixel (24 for all) */
    uint32_t biCompression;   /* 0x10: Compression (0 = BI_RGB) */
    uint32_t biSizeImage;     /* 0x14: Image data size */
    int32_t  biXPelsPerMeter; /* 0x18: X resolution (3790 or 3788) */
    int32_t  biYPelsPerMeter; /* 0x1C: Y resolution (3790 or 3788) */
    uint32_t biClrUsed;       /* 0x20: Colors used (0 for 24-bit) */
    uint32_t biClrImportant;  /* 0x24: Important colors (0) */
} SOTE_BITMAPINFOHEADER;
#endif

/* RIFF/WAVE Header - 12 bytes
 * Evidence: resource.catalog.json "RIFF (little-endian) data, WAVE audio"
 */
typedef struct {
    char     chunkID[4];    /* 0x00: 'RIFF' */
    uint32_t chunkSize;     /* 0x04: File size - 8 */
    char     format[4];     /* 0x08: 'WAVE' */
} RIFF_HEADER;

/* WAVE Format Chunk - 24 bytes
 * Evidence: resource.catalog.json "Microsoft PCM, 16 bit, mono"
 */
typedef struct {
    char     subchunk1ID[4];    /* 0x00: 'fmt ' */
    uint32_t subchunk1Size;     /* 0x04: 16 for PCM */
    uint16_t audioFormat;       /* 0x08: 1 = PCM */
    uint16_t numChannels;       /* 0x0A: 1 = mono */
    uint32_t sampleRate;        /* 0x0C: 11025 or 22050 Hz */
    uint32_t byteRate;          /* 0x10: SampleRate * NumChannels * BitsPerSample/8 */
    uint16_t blockAlign;        /* 0x14: NumChannels * BitsPerSample/8 (2) */
    uint16_t bitsPerSample;     /* 0x16: 16 bits */
} WAVE_FORMAT_CHUNK;

/* WAVE Data Chunk Header - 8 bytes */
typedef struct {
    char     subchunk2ID[4];    /* 0x00: 'data' */
    uint32_t subchunk2Size;     /* 0x04: Audio data size */
} WAVE_DATA_CHUNK;

/* PE DOS Header - 64 bytes
 * Evidence: Standard PE format for data00.dll-data31.dll loading
 */
typedef struct {
    uint16_t e_magic;       /* 0x00: MZ signature (0x5A4D) */
    uint16_t e_cblp;        /* 0x02: Bytes on last page */
    uint16_t e_cp;          /* 0x04: Pages in file */
    uint16_t e_crlc;        /* 0x06: Relocations */
    uint16_t e_cparhdr;     /* 0x08: Size of header in paragraphs */
    uint16_t e_minalloc;    /* 0x0A: Minimum extra paragraphs */
    uint16_t e_maxalloc;    /* 0x0C: Maximum extra paragraphs */
    uint16_t e_ss;          /* 0x0E: Initial SS */
    uint16_t e_sp;          /* 0x10: Initial SP */
    uint16_t e_csum;        /* 0x12: Checksum */
    uint16_t e_ip;          /* 0x14: Initial IP */
    uint16_t e_cs;          /* 0x16: Initial CS */
    uint16_t e_lfarlc;      /* 0x18: File address of relocation table */
    uint16_t e_ovno;        /* 0x1A: Overlay number */
    uint16_t e_res[4];      /* 0x1C: Reserved */
    uint16_t e_oemid;       /* 0x24: OEM identifier */
    uint16_t e_oeminfo;     /* 0x26: OEM information */
    uint16_t e_res2[10];    /* 0x28: Reserved */
    uint32_t e_lfanew;      /* 0x3C: Offset to PE header */
} PE_DOS_HEADER;

#pragma pack(pop)

/* Runtime structures - naturally aligned */

/* File Handle Wrapper
 * Evidence: CreateFileA @ 0x4c4634, ReadFile @ 0x4c4642, SetFilePointer usage
 */
typedef struct {
    void*    hFile;         /* 0x00: Windows file handle */
    uint32_t fileSize;      /* 0x04: File size in bytes */
    uint32_t currentPos;    /* 0x08: Current read position */
    void*    buffer;        /* 0x0C: Read buffer pointer */
    uint32_t bufferSize;    /* 0x10: Buffer size */
    uint32_t lastError;     /* 0x14: Last error code */
} FILE_HANDLE;

/* DLL Loader Context
 * Evidence: LoadLibraryA @ 0x4c450c, data00.dll-data31.dll loading
 */
typedef struct {
    void*    hModule;       /* 0x00: DLL module handle */
    char*    dllName;       /* 0x04: DLL name pointer */
    uint32_t refCount;      /* 0x08: Reference count */
    uint32_t loadFlags;     /* 0x0C: Load flags */
    void*    procAddress;   /* 0x10: GetProcAddress result */
} DLL_LOADER_CONTEXT;

/* SAN Movie Header - UNKNOWN STRUCTURE
 * Evidence: 17 .san movie files exist but format undocumented
 * NEEDS: Hex dump analysis of .san files to determine structure
 */
typedef struct {
    char     signature[4];   /* 0x00: SAN signature (UNKNOWN) */
    uint32_t version;        /* 0x04: Format version (UNKNOWN) */
    uint32_t frameCount;     /* 0x08: Number of frames (UNKNOWN) */
    uint32_t width;          /* 0x0C: Video width (UNKNOWN) */
    uint32_t height;         /* 0x10: Video height (UNKNOWN) */
    uint32_t fps;            /* 0x14: Frames per second (UNKNOWN) */
    uint32_t audioFormat;    /* 0x18: Audio format (UNKNOWN) */
    uint8_t  reserved[100];  /* 0x1C: Reserved/unknown fields */
} SAN_MOVIE_HEADER;

/* Missing Evidence - Commands needed to resolve unknowns:
 *
 * 1. SAN Movie Format:
 *    hexdump -C Sdata/intro.san | head -100
 *    radare2 -c "px 128" Sdata/intro.san
 *    
 * 2. Actual loader function analysis:
 *    radare2 -c "aa; axt @ sym.imp.CreateFileA" Shadows.exe
 *    radare2 -c "aa; pdf @ <loader_function_va>" Shadows.exe
 *    
 * 3. Memory access patterns for file parsing:
 *    radare2 -c "aa; /x 424d" Shadows.exe  # Find BMP header reads
 *    radare2 -c "aa; pd 50 @ <hit_address>" Shadows.exe
 *    
 * 4. Structure sizes from memcpy/malloc:
 *    radare2 -c "aa; /c push 0x36" Shadows.exe  # Find 54-byte BMP header allocs
 *    radare2 -c "aa; /c push 0x28" Shadows.exe  # Find 40-byte info header allocs
 */

#endif /* LOADER_STRUCTURES_H */