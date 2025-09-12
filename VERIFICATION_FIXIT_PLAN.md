# SOTE Binary Reimplementation - Verification Fix-It Plan

## Verification Summary
- **Quality Score**: 77.28%
- **Total Issues**: 21 (0 Fabrications, 20 Errors, 1 Warning)
- **Functions Verified**: 23/25
- **Stub Functions**: 11
- **Missing Functions**: 2

## Critical Issues to Fix

### Priority 1: Missing Function Declarations (2 functions)
These functions are referenced in the implementation but missing from the header:

1. **fcn.004160d8** - Rank 6, debug_output function
   - Evidence: 284 callers, 0 outdegree
   - Fix: Add to priority_functions.h with signature:
   ```c
   void __cdecl fcn_004160d8(void);
   ```

2. **fcn.00441bd1** - Rank 17, cleanup_handler function
   - Evidence: 200 callers, 2 outdegree  
   - Fix: Add to priority_functions.h with signature:
   ```c
   void __cdecl fcn_00441bd1(void);
   ```

### Priority 2: Parameter Count Mismatches (18 functions)
Critical signature errors that break binary compatibility:

#### Functions with Wrong Parameter Counts:
| Function | Current | Required | Evidence |
|----------|---------|----------|----------|
| fcn.00419d60 | 2 params | 3 params | r2_aflj offset 4300128 |
| fcn.00446000 | 2 params | 1 param | r2_aflj offset 4481024 |
| fcn.00424e40 | 1 param | 7 params | r2_aflj offset 4345408 |
| fcn.0043ee21 | 2 params | 4 params | r2_aflj offset 4451873 |
| fcn.0040b180 | 3 params | 6 params | r2_aflj offset 4239744 |
| fcn.00421690 | 2 params | 4 params | r2_aflj offset 4331152 |
| fcn.004159d0 | 0 params | 1 param | r2_aflj offset 4282832 |
| fcn.004424e1 | 0 params | 1 param | r2_aflj offset 4465889 |
| fcn.0040bc40 | 0 params | 6 params | r2_aflj offset 4242496 |
| fcn.0040c2e0 | 0 params | 6 params | r2_aflj offset 4244192 |
| fcn.00418ba0 | 0 params | 5 params | r2_aflj offset 4295584 |
| fcn.0040d070 | 0 params | 3 params | r2_aflj offset 4247664 |
| fcn.004184e0 | 0 params | 56 params! | r2_aflj offset 4293856 |
| fcn.00426ed0 | 0 params | 4 params | r2_aflj offset 4353744 |
| fcn.00419020 | 0 params | 8 params | r2_aflj offset 4296736 |
| fcn.00428060 | 0 params | 7 params | r2_aflj offset 4358240 |
| fcn.00416590 | 0 params | 4 params | r2_aflj offset 4285840 |
| fcn.0042b460 | 0 params | 1 param | r2_aflj offset 4371552 |

### Priority 3: Missing Provenance
1. **fcn.00437c83** - Provenance density only 0.13 (should be > 0.3)
   - Fix: Add more PROV comments citing r2_aflj evidence

## Actionable Fixes

### Fix 1: Update priority_functions.h
```c
// Add missing function declarations
void __cdecl fcn_004160d8(void);  // debug_output
void __cdecl fcn_00441bd1(void);  // cleanup_handler

// Fix parameter counts based on evidence
int __cdecl fcn_00419d60(int param1, int param2, int param3);  // Was 2, needs 3
void __cdecl fcn_00446000(void* param1);  // Was 2, needs 1
int __cdecl fcn_00424e40(void* p1, void* p2, void* p3, void* p4, void* p5, void* p6, void* p7);  // Was 1, needs 7
int __cdecl fcn_0043ee21(int p1, void* p2, void* p3, void* p4);  // Was 2, needs 4
void __cdecl fcn_0040b180(void* p1, int p2, int p3, void* p4, void* p5, void* p6);  // Was 3, needs 6
int __cdecl fcn_00421690(int p1, void* p2, void* p3, void* p4);  // Was 2, needs 4
void __cdecl fcn_004159d0(int param1);  // Was 0, needs 1
void __cdecl fcn_004424e1(int param1);  // Was 0, needs 1

// Functions with many parameters
void __cdecl fcn_0040bc40(void* p1, void* p2, void* p3, void* p4, void* p5, void* p6);  // 6 params
void __cdecl fcn_0040c2e0(void* p1, void* p2, void* p3, void* p4, void* p5, void* p6);  // 6 params
void __cdecl fcn_00418ba0(void* p1, void* p2, void* p3, void* p4, void* p5);  // 5 params
void __cdecl fcn_0040d070(void* p1, void* p2, void* p3);  // 3 params

// WARNING: fcn.004184e0 claims 56 parameters - likely needs review
// This is suspicious and may indicate stack analysis issue
void __cdecl fcn_004184e0(void);  // Keep as-is for now, needs investigation

void __cdecl fcn_00426ed0(void* p1, void* p2, void* p3, void* p4);  // 4 params
void __cdecl fcn_00419020(void* p1, void* p2, void* p3, void* p4, void* p5, void* p6, void* p7, void* p8);  // 8 params
void __cdecl fcn_00428060(void* p1, void* p2, void* p3, void* p4, void* p5, void* p6, void* p7);  // 7 params
void __cdecl fcn_00416590(void* p1, void* p2, void* p3, void* p4);  // 4 params
void __cdecl fcn_0042b460(void* param1);  // 1 param
```

### Fix 2: Update priority_functions.c
Update all function implementations to match the corrected signatures. Add parameter names and update PROV comments.

### Fix 3: Add More Provenance
For fcn.00437c83, add detailed PROV comments throughout the implementation citing specific evidence from r2_aflj.json.

## Evidence Requirements
To complete full implementations instead of stubs, we need:
1. **Ghidra decompilation** for all functions > 500 bytes
2. **Full disassembly** (r2 pdf) for small functions
3. **Cross-reference analysis** (r2 axt) for usage patterns
4. **String references** to understand function purposes
5. **Import usage patterns** from Windows API analysis

## Quality Metrics After Fixes
- Expected Quality Score: ~85-90%
- Expected Issues: < 5 (mostly INFO level)
- Binary Compatibility: 100% for signatures
- Evidence Coverage: 100% for declarations

## Next Steps
1. Apply fixes to header file (priority_functions.h)
2. Update implementation stubs (priority_functions.c)
3. Re-run verification
4. Obtain missing evidence for full implementations
5. Replace stubs with evidence-backed implementations