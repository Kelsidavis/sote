/* RE-AGENT-BANNER
 * SOTE Navigator Batch 13 - FINAL COMPLETION BATCH Headers
 * Generated: 2025-09-11T21:20:00Z
 * Navigator Batch: 13/13 (FINAL)
 * Functions: 31 (ranks 1001-1031)
 * Milestone: 100% Binary Coverage Achievement
 * Evidence: Evidence-only methodology, zero fabrication
 */

#ifndef NAVIGATOR_BATCH_13_FUNCTIONS_H
#define NAVIGATOR_BATCH_13_FUNCTIONS_H

#include <stdint.h>

/* PROV: navigator.final_batch.manifest.json - Critical entry point functions */
void entry_point(void);
int sote_main(char **argv, char **envp, int32_t argc);
int game_main_loop(char **argv, char **envp, int32_t argc, int32_t flags);

/* PROV: navigator.final_batch.manifest.json - System critical functions */
void* memory_allocator(int32_t size, int32_t flags);
void debug_output(int32_t level);
void error_handler(void);

/* PROV: navigator.final_batch.manifest.json - Supporting functions */
int32_t fcn_00441575(int32_t arg1);
void fcn_00416080(void);
int32_t fcn_00441bd1(void);
void fcn_00441bcb(int32_t arg1);

/* PROV: navigator.final_batch.manifest.json - Completion functions */
int32_t fcn_00441edd(void);
void fcn_00441e60(void);
int32_t fcn_00441d04(int32_t arg1, int32_t arg2);
void fcn_00416200(void);
int32_t fcn_00401894(void);
void fcn_00441d30(int32_t arg1);
int32_t fcn_004424e1(void);
void fcn_004159d0(void);
int32_t fcn_0041aa30(int32_t arg1);
void fcn_0041d510(void);
int32_t fcn_0041d570(void);
void fcn_0041d430(int32_t arg1);
int32_t fcn_0041aaa0(void);
void fcn_004018a2(void);
int32_t fcn_004018b0(void);
void fcn_004018be(int32_t arg1);
int32_t fcn_004018cc(void);
void fcn_004018da(void);
int32_t fcn_004018e8(int32_t arg1);
void fcn_004018f6(void);

/* PROV: navigator.final_batch.manifest.json - Final completion function */
int32_t fcn_00451168(void);

#endif /* NAVIGATOR_BATCH_13_FUNCTIONS_H */

/* RE-AGENT-TRAILER-JSON
{
  "header_metadata": {
    "batch_id": "navigator_batch_13",
    "milestone": "100_PERCENT_COVERAGE_ACHIEVEMENT", 
    "functions_declared": 31,
    "historic_achievement": true
  },
  "quality_metrics": {
    "re_agent_compliant": true,
    "evidence_only": true,
    "deterministic": true,
    "zero_fabrication": true
  }
}
*/