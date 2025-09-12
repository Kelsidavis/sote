# Stage 4: Loader Layout Analysis Report

## Summary
Mechanically derived loader structures for SOTE runtime enablement based on:
- Binary evidence from Shadows.exe (SHA256: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77)
- Resource catalog analysis of 277 assets (43 BMPs, 183 WAVs, 17 SANs, 34 DLLs)
- Import table analysis showing file I/O functions at known addresses

## Structures Derived

### 1. Confirmed with High Confidence
- **BITMAPFILEHEADER** (14 bytes): Full structure derived from format spec + resource evidence
- **BITMAPINFOHEADER** (40 bytes): Windows 3.x format confirmed for all 43 bitmaps
- **RIFF_HEADER** (12 bytes): Standard RIFF container for WAV files
- **WAVE_FORMAT_CHUNK** (24 bytes): PCM audio format, 16-bit mono confirmed
- **WAVE_DATA_CHUNK** (8 bytes): Standard WAV data chunk header
- **PE_DOS_HEADER** (64 bytes): Standard PE format for DLL loading

### 2. Inferred with Medium Confidence
- **FILE_HANDLE** (24 bytes): Runtime wrapper around Windows file operations
- **DLL_LOADER_CONTEXT** (20 bytes): Context for loading data00.dll-data31.dll

### 3. Unknown - Requires Additional Evidence
- **SAN_MOVIE_HEADER**: LucasArts SAN format undocumented, needs hex analysis

## Evidence Sources

### Direct Binary Evidence
```
VA_0x4dba3b: BMP magic number 'BM' (0x424D) found
VA_0x4c4634: CreateFileA import address
VA_0x4c4642: ReadFile import address  
VA_0x4c450c: LoadLibraryA import address
```

### Resource Catalog Evidence
- All BMPs: Windows 3.x format, 24-bit color, 54-byte offset to pixels
- All WAVs: RIFF little-endian, Microsoft PCM, 16-bit mono
- Sample rates: 11025 Hz or 22050 Hz only
- BMP resolutions: 3790x3790 or 3788x3788 px/m

## Missing Evidence - Commands to Execute

### 1. SAN Movie Format Analysis
```bash
# Examine SAN file headers
hexdump -C Sdata/intro.san | head -50
hexdump -C Sdata/finale.san | head -50

# Look for consistent magic/signature
for f in Sdata/*.san; do hexdump -C "$f" | head -1; done | sort | uniq

# Radare2 structure analysis
radare2 -q -c "px 256" Sdata/intro.san
```

### 2. Loader Function Disassembly
```bash
# Find CreateFileA call sites
radare2 -q -c "aa; axt @ 0x4c4634" Sdata/Shadows.exe

# Find ReadFile call sites  
radare2 -q -c "aa; axt @ 0x4c4642" Sdata/Shadows.exe

# Analyze file loading functions
radare2 -q -c "aa; /x ff15344646" Sdata/Shadows.exe  # calls to CreateFileA
radare2 -q -c "aa; /x ff15424646" Sdata/Shadows.exe  # calls to ReadFile
```

### 3. Structure Size Evidence
```bash
# Find allocations matching structure sizes
radare2 -q -c "aa; /x 6a36" Sdata/Shadows.exe  # push 54 (BMP header size)
radare2 -q -c "aa; /x 6a28" Sdata/Shadows.exe  # push 40 (info header size)
radare2 -q -c "aa; /x 6a0c" Sdata/Shadows.exe  # push 12 (RIFF header size)

# Find memcpy operations with these sizes
radare2 -q -c "aa; /c mov ecx, 0x36" Sdata/Shadows.exe
radare2 -q -c "aa; /c mov ecx, 0x28" Sdata/Shadows.exe
```

### 4. Memory Access Pattern Analysis
```bash
# Find code that reads at specific offsets
radare2 -q -c "aa; /x 8b4002" Sdata/Shadows.exe  # mov eax, [eax+0x2] (bfSize)
radare2 -q -c "aa; /x 8b400a" Sdata/Shadows.exe  # mov eax, [eax+0xa] (bfOffBits)
radare2 -q -c "aa; /x 8b4604" Sdata/Shadows.exe  # mov eax, [esi+0x4] (biWidth)
radare2 -q -c "aa; /x 8b4608" Sdata/Shadows.exe  # mov eax, [esi+0x8] (biHeight)
```

### 5. DLL Loading Pattern Analysis
```bash
# Find LoadLibraryA calls with data*.dll strings
radare2 -q -c "iz~data[0-9][0-9].dll" Sdata/Shadows.exe

# Find GetProcAddress usage after LoadLibrary
radare2 -q -c "aa; axt @ sym.imp.GetProcAddress" Sdata/Shadows.exe
```

## Validation Against Runtime Requirements

### File I/O APIs (from runtime.apis.json)
- CreateFileA @ 0x4c4634 ✓ Confirmed in IAT
- ReadFile @ 0x4c4642 ✓ Confirmed in IAT  
- SetFilePointer ✓ Expected for sequential reads
- CloseHandle ✓ Standard cleanup

### Resource Loading Sequences
1. BMP: CreateFileA → ReadFile(54) → Validate headers → ReadFile(pixels)
2. WAV: CreateFileA → ReadFile(44) → Parse RIFF/fmt → ReadFile(audio)
3. DLL: LoadLibraryA("data00.dll") → GetProcAddress → Call functions
4. SAN: UNKNOWN - needs format documentation

## Integration Notes for Stage 6

The derived structures in `include/loader_structures.h` provide:
1. Exact byte layouts for file format parsing
2. Runtime wrappers for Windows API interactions
3. Clear provenance for each field via layouts.curated.json

Missing SAN format requires either:
- Reverse engineering from binary analysis
- Finding LucasArts format documentation
- Hex comparison of the 17 SAN files to derive structure

## Quality Metrics
- Fields with provenance: 100% (all have PROV tags)
- Structures with high confidence: 6/9 (67%)
- Structures with medium confidence: 2/9 (22%)  
- Structures needing evidence: 1/9 (11%)

## Next Steps
1. Execute missing evidence commands above
2. Analyze SAN file format through hex dumps
3. Trace actual loader function implementations
4. Update structures with memory access evidence
5. Proceed to Stage 6 runtime implementation