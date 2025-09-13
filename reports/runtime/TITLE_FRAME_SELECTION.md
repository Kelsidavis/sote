# Title Frame Selection Report

**Agent**: resource-runtime-annotator  
**Call ID**: coord-startup-003  
**Timestamp**: 2025-09-12T21:45:00Z

## Analysis Summary

### Search Strategy
The resource annotator searched for title frame assets using the following priority:

1. **Direct candidates**: TITLE.BMP, TITLE_PC.BMP, art/title.bmp, front/title.bmp, splash.bmp, logos/title.png
2. **Fallback patterns**: `**/title*.bmp`, `**/splash*.bmp`, `**/logo*.bmp`
3. **Discovery scan**: All image and video assets in `/media/k/vbox1/Shadows/SOTE/Sdata`

### Search Results

#### Direct Candidates: NOT FOUND
None of the expected title image files were present in the asset directory.

#### Fallback Patterns: NO MATCHES  
No files matching title/splash/logo naming patterns were found.

#### Asset Discovery: SUCCESS

**Primary Selection**: `l00logo.san`
- **Status**: FOUND ✓
- **Path**: `/media/k/vbox1/Shadows/SOTE/Sdata/l00logo.san`
- **Size**: 9,275,340 bytes
- **Format**: SAN video format
- **Classification**: Logo video sequence
- **Confidence**: HIGH

**Fallback Selection**: `boba.bmp`
- **Status**: FOUND ✓
- **Path**: `/media/k/vbox1/Shadows/SOTE/Sdata/boba.bmp`  
- **Size**: 921,654 bytes
- **Format**: Windows Bitmap
- **Classification**: Character/menu background
- **Confidence**: MEDIUM

## Runtime Configuration

### Selected Title Sequence
```
PRIMARY:  /media/k/vbox1/Shadows/SOTE/Sdata/l00logo.san
FALLBACK: /media/k/vbox1/Shadows/SOTE/Sdata/boba.bmp
BASE_DIR: /media/k/vbox1/Shadows/SOTE/Sdata
```

### Render Strategy
1. **Primary**: Attempt SAN video decode/playback of `l00logo.san`
2. **Fallback**: Display bitmap `boba.bmp` if SAN decoder unavailable
3. **Expected Log**: `[RENDER] title candidate: l00logo.san`

## Verification Requirements

### File Accessibility
- ✓ Assets directory exists: `/media/k/vbox1/Shadows/SOTE/Sdata`
- ✓ Logo video exists: `l00logo.san` (9.3MB)
- ✓ Fallback bitmap exists: `boba.bmp` (921KB)

### Runtime Requirements
- **SAN Decoder**: Required for primary title sequence
- **BMP Loader**: Required for fallback display
- **SDL Surface**: Target render surface for image display

### Expected Trace Output
```
[DISCLESS] Video adapter: SDL
[RENDER] title candidate: l00logo.san
[RENDER] Loading title sequence from Sdata/l00logo.san
```

## Implementation Notes

The selected title assets provide a robust two-tier approach:
1. **l00logo.san**: Authentic game logo video (if decoder available)
2. **boba.bmp**: Character artwork fallback (universally supported)

This ensures title screen capability regardless of video decoder availability in the disc-less/Wine environment.

---
**Selection Status**: ✓ COMPLETE  
**Confidence Level**: HIGH  
**Next Step**: Wire SDL draw path for title display