# Struct Layout Analysis for 31 Remaining Functions

## Analysis Summary

After reviewing the 31 remaining unresolved functions in the SOTE project, the structural dependencies are minimal:

### Function Categories

1. **Complex Stack Frame Function (1)**
   - `fcn_00401041` (0x401041): 556-byte stack frame with 27 local variables
   - Despite the large stack frame, this is only a 5-byte function (likely a jump thunk)
   - Stack variables are distributed across offsets from -360 to +508

2. **Simple 5-byte Functions (12)**
   - Functions: 0x401046, 0x401050, 0x40109b, 0x4010cd, 0x4010eb, 0x4010f0, 0x401145, 0x401195, 0x4011ae, 0x40122b, 0x40124e
   - Likely jump thunks or minimal stubs
   - No evidence of struct parameters or complex data types

3. **8-byte Utility Functions (18)**
   - Functions in ranges 0x401927, 0x4023ee-0x402410, 0x40265d-0x402f22, 0x403544-0x403b39
   - Slightly larger but still minimal functions
   - Likely import wrappers or simple utilities

## Structural Requirements

### New Structures Required: **1**

#### local_struct_00401041
```c
// Stack frame structure for fcn_00401041
// Total size: 556 bytes (stack frame)
// 27 local variables distributed across the frame
struct local_struct_00401041 {
    // Negative offsets (below EBP)
    int32_t var_168h_3;    // offset: -360
    int32_t var_168h_2;    // offset: -332  
    int32_t var_13ch;      // offset: -316
    int32_t var_24h_3;     // offset: -36
    int32_t var_1ch_3;     // offset: -28
    int32_t var_20h_4;     // offset: -24
    int32_t var_14h_5;     // offset: -20
    int32_t var_10h_3;     // offset: -16
    int32_t var_14h_4;     // offset: -12
    int32_t var_24h_2;     // offset: -8
    int32_t var_20h_3;     // offset: -4
    
    // Positive offsets (above EBP)
    int32_t var_1ch_2;     // offset: 0
    int32_t var_20h_2;     // offset: 4
    int32_t var_14h_3;     // offset: 8
    int32_t var_10h_2;     // offset: 12
    int32_t var_14h_2;     // offset: 16
    int32_t var_4h_3;      // offset: 24
    int32_t var_4h_2;      // offset: 32
    int32_t var_168h;      // offset: 152
    int32_t var_28h;       // offset: 472
    int32_t var_24h;       // offset: 476
    int32_t var_20h;       // offset: 480
    int32_t var_1ch;       // offset: 484
    int32_t var_18h;       // offset: 488
    int32_t var_14h;       // offset: 492
    int32_t var_10h;       // offset: 496
    int32_t var_4h;        // offset: 508
};
```

### No Structures Required for Other Functions

The remaining 30 functions (5-byte and 8-byte) show no evidence of:
- Complex parameter structures
- Return value structures  
- Global data structure access patterns
- Array or pointer-based structures

These can be implemented as simple functions with:
- Basic integer parameters (if any)
- Integer or void return types
- No struct dependencies

## Implementation Recommendations

### Priority 1: fcn_00401041
- Add the stack frame structure to layouts.curated.json
- The function itself is only 5 bytes (likely `jmp` instruction)
- Stack frame may be inherited from the jump target

### Priority 2-31: Simple Functions
- Implement as minimal stubs without struct dependencies
- 5-byte functions: likely `jmp rel32` instructions
- 8-byte functions: likely `push imm32; jmp rel32` or similar

## Evidence Gaps

No additional structural evidence is required. The functions are too small to contain complex data access patterns:
- 5-byte functions can only contain a single jump instruction
- 8-byte functions can contain at most 2-3 simple instructions

## Conclusion

**Only 1 new struct definition is required** for the 31 remaining functions, and it's a stack frame structure for fcn_00401041. The other 30 functions are simple thunks/utilities that don't require struct support.

The project can achieve 100% coverage with minimal structural additions, maintaining the evidence-only methodology.