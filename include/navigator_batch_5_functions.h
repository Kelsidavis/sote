/*
 * ========================================================================
 * RE-AGENT NAVIGATOR BATCH 5 - HIGH PRIORITY FUNCTIONS
 * ========================================================================
 * 
 * PROJECT: SOTE.EXE Reverse Engineering
 * ARTIFACT: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77
 * ARCH/ABI: i386/pe32
 * BATCH: Navigator Batch 5 (Ranks 261-330)
 * FUNCTIONS: 70 high priority hub and processing functions
 * 
 * PROVENANCE:
 * - expansion_targets_next200.json: Function rankings and connectivity metrics
 * - inputs/r2_aflj.json: Binary-extracted function signatures  
 * - navigator.next_batch_2.manifest.json: Batch assignment and prioritization
 * 
 * QUALITY METRICS:
 * - Average Hotness Score: 28.7
 * - Total Estimated Cost: 15234
 * - Average Connectivity: >100 (high)
 * - Evidence Density: 0.73+ (medium-high confidence)
 * 
 * ========================================================================
 */

#ifndef NAVIGATOR_BATCH_5_FUNCTIONS_H
#define NAVIGATOR_BATCH_5_FUNCTIONS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * HIGH CONNECTIVITY PROCESSING HUBS (Ranks 261-270)
 * ======================================================================== */

/* Rank 261: fcn_00428060 - Processing hub with moderate complexity (connectivity: 90) */
/* PROV: r2 @ 0x428060, size: 749, cc: 49 */
void fcn_00428060(int32_t arg_38h_2, int32_t arg_38h, int32_t arg_3ch, 
                  int32_t arg_40h, int32_t arg_44h, int32_t arg_48h, 
                  uint32_t arg_4ch);

/* Rank 262: fcn_00401159 - Large dispatcher function (connectivity: 108) */
/* PROV: r2 @ 0x401159, size: 22145, cc: 21 */
// PROV: canonical fcn_00401159 from sote_symbols.h
// PROV: Removed redundant declaration - canonical in sote_symbols.h

/* Rank 263: fcn_00426900 - Balanced hub with good connectivity (connectivity: 97) */
/* PROV: r2 @ 0x426900, size: 704, cc: 31 */
void fcn_00426900(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch, 
                  int32_t arg_50h, int32_t arg_54h);

/* Rank 264: fcn_00427520 - Processing function with balanced I/O (connectivity: 94) */
/* PROV: r2 @ 0x427520, size: 518, cc: 36 */
void fcn_00427520(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 265: fcn_0041c950 - Connector function (connectivity: 95) */
/* PROV: r2 @ 0x41c950, size: 411, cc: 31 */
void fcn_0041c950(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h);

/* Rank 266: fcn_0042d0f4 - Dispatcher with good fan-out (connectivity: 92) */
/* PROV: r2 @ 0x42d0f4, size: 832, cc: 32 */
void fcn_0042d0f4(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch, 
                  int32_t arg_50h);

/* Rank 267: loc.00406b50 - High fan-out dispatcher (connectivity: 104) */
/* PROV: r2 @ 0x406b50, size: 1111, cc: 17 */
void loc_00406b50(int32_t arg_8h, int32_t arg_ch, int32_t arg_10h, 
                  int32_t arg_14h, int32_t arg_18h, int32_t arg_1ch);

/* Rank 268: fcn_004255e0 - Processing hub (connectivity: 83) */
/* PROV: r2 @ 0x4255e0, size: 923, cc: 36 */
void fcn_004255e0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch, 
                  int32_t arg_50h);

/* Rank 269: fcn_00410260 - Balanced processing function (connectivity: 84) */
/* PROV: r2 @ 0x410260, size: 631, cc: 34 */
void fcn_00410260(int32_t arg_ch, int32_t arg_10h, int32_t arg_14h);

/* Rank 270: fcn_004242c0 - Processing component (connectivity: 77) */
/* PROV: r2 @ 0x4242c0, size: 767, cc: 40 */
void fcn_004242c0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch, 
                  int32_t arg_50h, int32_t arg_54h);

/* ========================================================================
 * MEDIUM COMPLEXITY PROCESSING FUNCTIONS (Ranks 271-280)
 * ======================================================================== */

/* Rank 271: fcn_0044f383 - Connector with balanced I/O (connectivity: 86) */
/* PROV: r2 @ 0x44f383, size: 541, cc: 25 */
void fcn_0044f383(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 272: fcn_0041ece0 - Processing function (connectivity: 78) */
/* PROV: r2 @ 0x41ece0, size: 735, cc: 32 */
void fcn_0041ece0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 273: fcn_00422f60 - Medium complexity component (connectivity: 71) */
/* PROV: r2 @ 0x422f60, size: 618, cc: 42 */
void fcn_00422f60(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 274: fcn_0041c720 - Balanced connector (connectivity: 76) */
/* PROV: r2 @ 0x41c720, size: 560, cc: 30 */
void fcn_0041c720(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 275: fcn_0042bc90 - Dispatcher function (connectivity: 82) */
/* PROV: r2 @ 0x42bc90, size: 822, cc: 23 */
void fcn_0042bc90(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch, 
                  int32_t arg_50h);

/* Rank 276: fcn_00427750 - Processing function (connectivity: 71) */
/* PROV: r2 @ 0x427750, size: 678, cc: 35 */
void fcn_00427750(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 277: fcn_00409020 - Large processing function (connectivity: 71) */
// PROV: canonical fcn_00409020 from sote_symbols.h
/* PROV: r2 @ 0x409020, size: 1239, cc: 35 */
// PROV: Removed redundant declaration - canonical in sote_symbols.h

/* Rank 278: fcn_00422030 - High fan-in utility (connectivity: 74) */
/* PROV: r2 @ 0x422030, size: 279, cc: 18 */
void fcn_00422030(int32_t arg_28h, int32_t arg_2ch, int32_t arg_30h);

/* Rank 279: fcn_004249f0 - Processing component (connectivity: 71) */
/* PROV: r2 @ 0x4249f0, size: 424, cc: 31 */
void fcn_004249f0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 280: fcn_0041d5c0 - Medium complexity function (connectivity: 65) */
/* PROV: r2 @ 0x41d5c0, size: 634, cc: 35 */
void fcn_0041d5c0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* ========================================================================
 * CONNECTOR AND UTILITY FUNCTIONS (Ranks 281-290)
 * ======================================================================== */

/* Rank 281: fcn_00415c60 - Utility function (connectivity: 68) */
/* PROV: r2 @ 0x415c60, size: 254, cc: 27 */
void fcn_00415c60(int32_t arg_ch, int32_t arg_10h, int32_t arg_14h);

/* Rank 282: fcn_0044176e - Small connector (connectivity: 56) */
/* PROV: r2 @ 0x44176e, size: 242, cc: 50 */
void fcn_0044176e(int32_t arg_8h);

/* Rank 283: fcn_004421a8 - High fan-in terminal (connectivity: 74) */
/* PROV: r2 @ 0x4421a8, size: 40, cc: 2 */
void fcn_004421a8(int32_t arg_38h);

/* Rank 284: fcn_0041aaa0 - Small utility (connectivity: 73) */
/* PROV: r2 @ 0x41aaa0, size: 23, cc: 1 */
void fcn_0041aaa0(void);

/* Rank 285: fcn_00428380 - Processing function (connectivity: 65) */
/* PROV: r2 @ 0x428380, size: 592, cc: 32 */
void fcn_00428380(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 286: fcn_0041b6b0 - Connector function (connectivity: 65) */
/* PROV: r2 @ 0x41b6b0, size: 480, cc: 25 */
void fcn_0041b6b0(int32_t arg_8h, int32_t arg_ch, int32_t arg_10h, 
                  int32_t arg_14h);

/* Rank 287: fcn_004202d0 - Processing component (connectivity: 64) */
/* PROV: r2 @ 0x4202d0, size: 555, cc: 26 */
void fcn_004202d0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 288: fcn_0043cfc3 - Dispatcher (connectivity: 68) */
/* PROV: r2 @ 0x43cfc3, size: 558, cc: 19 */
void fcn_0043cfc3(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 289: fcn_00419720 - Processing function (connectivity: 68) */
/* PROV: r2 @ 0x419720, size: 510, cc: 19 */
void fcn_00419720(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 290: fcn_00409fe0 - Large processing function (connectivity: 55) */
/* PROV: r2 @ 0x409fe0, size: 754, cc: 35 */
void fcn_00409fe0(int32_t arg_8h, int32_t arg_ch, int32_t arg_10h, 
                  int32_t arg_14h);

/* ========================================================================
 * PROCESSING AND HELPER FUNCTIONS (Ranks 291-300)
 * ======================================================================== */

/* Rank 291: fcn_0040abe0 - Processing function (connectivity: 60) */
/* PROV: r2 @ 0x40abe0, size: 462, cc: 27 */
void fcn_0040abe0(int32_t arg_1ch);

/* Rank 292: fcn_00416200 - Utility function (connectivity: 58) */
/* PROV: r2 @ 0x416200, size: 334, cc: 26 */
void fcn_00416200(int32_t arg_ch, int32_t arg_10h, int32_t arg_14h);

/* Rank 293: fcn_0040b9a0 - Processing function (connectivity: 57) */
/* PROV: r2 @ 0x40b9a0, size: 671, cc: 30 */
void fcn_0040b9a0(int32_t arg_ch, int32_t arg_10h, int32_t arg_14h, 
                  int32_t arg_18h, int32_t arg_1ch);

/* Rank 294: fcn_0041d840 - Processing component (connectivity: 58) */
/* PROV: r2 @ 0x41d840, size: 380, cc: 27 */
void fcn_0041d840(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h);

/* Rank 295: fcn_00441edd - Small utility with high fan-in (connectivity: 65) */
/* PROV: r2 @ 0x441edd, size: 40, cc: 2 */
void fcn_00441edd(int32_t arg_8h);

/* Rank 296: fcn_00420080 - Processing function (connectivity: 58) */
/* PROV: r2 @ 0x420080, size: 524, cc: 25 */
void fcn_00420080(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 297: fcn_00410ed0 - Processing component (connectivity: 58) */
/* PROV: r2 @ 0x410ed0, size: 478, cc: 20 */
void fcn_00410ed0(int32_t arg_ch, int32_t arg_10h, int32_t arg_14h, 
                  int32_t arg_18h);

/* Rank 298: fcn_0041aa30 - Small utility with high fan-in (connectivity: 62) */
/* PROV: r2 @ 0x41aa30, size: 104, cc: 4 */
void fcn_0041aa30(int32_t arg_8h);

/* Rank 299: fcn_00415530 - Processing function (connectivity: 54) */
/* PROV: r2 @ 0x415530, size: 418, cc: 27 */
void fcn_00415530(int32_t arg_ch, int32_t arg_10h, int32_t arg_14h, 
                  int32_t arg_18h);

/* Rank 300: fcn_0041b520 - Connector function (connectivity: 54) */
/* PROV: r2 @ 0x41b520, size: 396, cc: 27 */
void fcn_0041b520(int32_t arg_8h, int32_t arg_ch, int32_t arg_10h, 
                  int32_t arg_14h);

/* ========================================================================
 * LOWER COMPLEXITY UTILITY FUNCTIONS (Ranks 301-310)
 * ======================================================================== */

/* Rank 301: fcn_0040a420 - Processing function (connectivity: 56) */
/* PROV: r2 @ 0x40a420, size: 597, cc: 21 */
void fcn_0040a420(int32_t arg_1ch);

/* Rank 302: fcn_0044164a - Connector function (connectivity: 56) */
/* PROV: r2 @ 0x44164a, size: 292, cc: 21 */
void fcn_0044164a(int32_t arg_8h);

/* Rank 303: fcn_00417280 - Processing function (connectivity: 55) */
/* PROV: r2 @ 0x417280, size: 547, cc: 25 */
void fcn_00417280(int32_t arg_1ch, int32_t arg_20h, int32_t arg_24h, 
                  int32_t arg_28h);

/* Rank 304: fcn_00414b40 - Utility function (connectivity: 55) */
/* PROV: r2 @ 0x414b40, size: 383, cc: 22 */
void fcn_00414b40(int32_t arg_ch, int32_t arg_10h, int32_t arg_14h);

/* Rank 305: fcn_00415900 - Small terminal utility (connectivity: 59) */
/* PROV: r2 @ 0x415900, size: 20, cc: 1 */
void fcn_00415900(void);

/* Rank 306: fcn_0043e483 - Small utility (connectivity: 57) */
/* PROV: r2 @ 0x43e483, size: 131, cc: 9 */
void fcn_0043e483(int32_t arg_38h);

/* Rank 307: fcn_0042c150 - Processing function (connectivity: 56) */
/* PROV: r2 @ 0x42c150, size: 558, cc: 21 */
void fcn_0042c150(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 308: fcn_0042b120 - Large processing function (connectivity: 59) */
/* PROV: r2 @ 0x42b120, size: 831, cc: 16 */
void fcn_0042b120(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch, 
                  int32_t arg_50h);

/* Rank 309: fcn_0044f0a8 - Connector function (connectivity: 58) */
/* PROV: r2 @ 0x44f0a8, size: 443, cc: 17 */
void fcn_0044f0a8(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 310: fcn_00415a40 - Processing function (connectivity: 55) */
/* PROV: r2 @ 0x415a40, size: 450, cc: 20 */
void fcn_00415a40(int32_t arg_ch, int32_t arg_10h, int32_t arg_14h, 
                  int32_t arg_18h);

/* ========================================================================
 * SPECIALIZED UTILITY FUNCTIONS (Ranks 311-320)
 * ======================================================================== */

/* Rank 311: fcn_0044f263 - Connector function (connectivity: 58) */
/* PROV: r2 @ 0x44f263, size: 288, cc: 16 */
void fcn_0044f263(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 312: fcn_00444d53 - Utility function (connectivity: 54) */
/* PROV: r2 @ 0x444d53, size: 233, cc: 22 */
void fcn_00444d53(int32_t arg_38h, int32_t arg_3ch);

/* Rank 313: fcn_00440adb - Processing function (connectivity: 55) */
/* PROV: r2 @ 0x440adb, size: 350, cc: 19 */
void fcn_00440adb(int32_t arg_38h, int32_t arg_3ch);

/* Rank 314: fcn_0043dd4b - Small terminal utility (connectivity: 57) */
/* PROV: r2 @ 0x43dd4b, size: 42, cc: 1 */
void fcn_0043dd4b(void);

/* Rank 315: fcn_00440c64 - Connector function (connectivity: 56) */
/* PROV: r2 @ 0x440c64, size: 328, cc: 12 */
void fcn_00440c64(int32_t arg_38h, int32_t arg_3ch);

/* Rank 316: fcn_0041fa10 - Processing function (connectivity: 49) */
/* PROV: r2 @ 0x41fa10, size: 458, cc: 25 */
void fcn_0041fa10(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 317: fcn_0043d3cf - Processing function (connectivity: 57) */
/* PROV: r2 @ 0x43d3cf, size: 375, cc: 13 */
void fcn_0043d3cf(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h);

/* Rank 318: fcn_0042bfd0 - Connector function (connectivity: 51) */
/* PROV: r2 @ 0x42bfd0, size: 335, cc: 21 */
void fcn_0042bfd0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 319: fcn_00417010 - Utility function (connectivity: 46) */
/* PROV: r2 @ 0x417010, size: 334, cc: 26 */
void fcn_00417010(int32_t arg_1ch, int32_t arg_20h, int32_t arg_24h);

/* Rank 320: fcn_0044070f - Processing function (connectivity: 54) */
/* PROV: r2 @ 0x44070f, size: 380, cc: 15 */
void fcn_0044070f(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h);

/* ========================================================================
 * FINAL BATCH FUNCTIONS (Ranks 321-330)
 * ======================================================================== */

/* Rank 321: fcn_00422dc0 - Utility function (connectivity: 51) */
/* PROV: r2 @ 0x422dc0, size: 255, cc: 19 */
void fcn_00422dc0(int32_t arg_2ch, int32_t arg_30h, int32_t arg_34h);

/* Rank 322: fcn_0043fcbd - Processing function (connectivity: 53) */
/* PROV: r2 @ 0x43fcbd, size: 324, cc: 15 */
void fcn_0043fcbd(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h);

/* Rank 323: fcn_0041eff0 - Processing function (connectivity: 46) */
/* PROV: r2 @ 0x41eff0, size: 603, cc: 25 */
void fcn_0041eff0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 324: fcn_00421d60 - Processing function (connectivity: 42) */
/* PROV: r2 @ 0x421d60, size: 584, cc: 30 */
void fcn_00421d60(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h, int32_t arg_4ch);

/* Rank 325: fcn_00441e8d - Small utility (connectivity: 52) */
/* PROV: r2 @ 0x441e8d, size: 80, cc: 5 */
void fcn_00441e8d(int32_t arg_8h);

/* Rank 326: fcn_0041ac90 - Processing function (connectivity: 47) */
/* PROV: r2 @ 0x41ac90, size: 335, cc: 18 */
void fcn_0041ac90(int32_t arg_8h, int32_t arg_ch, int32_t arg_10h, 
                  int32_t arg_14h);

/* Rank 327: fcn_0041ccd0 - Processing function (connectivity: 45) */
/* PROV: r2 @ 0x41ccd0, size: 507, cc: 21 */
void fcn_0041ccd0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 328: fcn_0042d4c0 - Processing function (connectivity: 46) */
/* PROV: r2 @ 0x42d4c0, size: 342, cc: 19 */
void fcn_0042d4c0(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h, 
                  int32_t arg_44h, int32_t arg_48h);

/* Rank 329: fcn_00445f5f - Small utility (connectivity: 34) */
/* PROV: r2 @ 0x445f5f, size: 81, cc: 39 */
void fcn_00445f5f(void);

/* Rank 330: fcn_00442c75 - Processing function (connectivity: 48) */
/* PROV: r2 @ 0x442c75, size: 289, cc: 16 */
void fcn_00442c75(int32_t arg_38h, int32_t arg_3ch, int32_t arg_40h);

#ifdef __cplusplus
}
#endif

#endif /* NAVIGATOR_BATCH_5_FUNCTIONS_H */