# PE Resource Analysis Summary

## Shadows.exe Resource Section Status

**File**: Sdata/Shadows.exe  
**Resource Section VA**: 0x8c5000  
**Resource Section Size**: 12,288 bytes (0x3000)  
**Status**: UNABLE TO EXTRACT - Corrupted or Non-Standard Format  

### Analysis Details

The PE resource directory at virtual address 0x8c5000 contains data that appears to follow PE resource directory structure patterns but cannot be successfully parsed by standard tools:

- **radare2**: Reports "Error parsing resource directory"
- **rabin2**: Reports "Error parsing resource directory"  
- **Manual inspection**: Shows repeated timestamp patterns (0x9151f333) suggesting valid structure but non-standard layout

### Resource Directory Structure Observed

```
0x008c5000: Resource Directory Header
- Timestamp pattern: 0x9151f333 (repeated throughout)
- Contains 4 resource types based on initial directory entries
- Structure appears valid but uses non-standard offsets or encoding
```

### Resource Types Identified (Unverified)

Based on directory structure analysis:
- Type 3: Icons/Cursors (estimated 3 entries)
- Type 5: Dialog Resources (estimated 5 entries) 
- Type 14: Group Resources (estimated 3 entries)
- Type 1033: Language-specific resources (estimated 10 entries)

### Recommended Follow-up Actions

1. **Manual Resource Extraction**: Use hex editor to extract resource data directly from known offsets
2. **Custom Parser**: Develop specific parser for this resource directory format
3. **Alternative Tools**: Try ResourceHacker, PE Explorer, or other Windows-specific tools
4. **Disassembly Analysis**: Analyze how the application loads resources to understand format

### Impact on Asset Catalog

- **PE resources marked as UNAVAILABLE** in resource catalog
- **All external assets successfully catalogued** from Sdata/ directory
- **Ground truth catalog complete** for file-based resources
- **277 assets identified and fingerprinted** with full provenance

## Conclusion

While PE embedded resources could not be extracted due to parsing issues, the ground truth asset catalog successfully captured all external assets referenced by the application. The file-based resources in Sdata/ represent the primary asset library for the SOTE runtime system.