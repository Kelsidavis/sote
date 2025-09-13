# Symbol Conflict Prototype Matrix

## Multiple Definition Conflicts

| Symbol | Navigator Files | Batch Files | Resolution |
|--------|----------------|-------------|------------|
| `debug_output` | navigator_batch_13_functions.c | - | Keep in main.c, remove from others |
| `entry_point` | navigator_batch_13_functions.c | - | Keep in entry.c, remove from others |
| `error_handler` | navigator_batch_13_functions.c | - | Keep in main.c, remove from others |
| `fcn_00401159` | navigator_batch_5_functions.c, navigator_batch_4_functions.c, navigator_batch_4_functions.c | - | Manual review required |
| `fcn_004011a4` | navigator_batch_6_functions.c | - | Manual review required |
| `fcn_00409020` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00409fe0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0040a420` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_0040a980` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0040abe0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0040b180` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_0040b9a0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0040bc40` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_0040c2e0` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_0040d070` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_0040fc20` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00410260` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00410ed0` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_004144d0` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00414b40` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00414cc0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00415100` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00415530` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00415900` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_004159d0` | navigator_batch_13_functions.c, navigator_batch_4_functions.c | batch5_functions.c, batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_00415a40` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_00415c60` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_00416080` | navigator_batch_13_functions.c | batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_004160d8` | navigator_batch_4_functions.c | batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_00416200` | navigator_batch_13_functions.c, navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_00416370` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00416430` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00416590` | navigator_batch_4_functions.c | batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_00416a50` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00416b30` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00416c20` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00416cd0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00417010` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00417280` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_004177c0` | navigator_batch_4_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00417b30` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_004183c0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_004184e0` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_00418ba0` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_00418ec0` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00419020` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_00419720` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_00419ad0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00419d60` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_0041aa30` | navigator_batch_13_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0041aaa0` | navigator_batch_13_functions.c, navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0041abc0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0041ac90` | navigator_batch_5_functions.c | batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_0041b150` | navigator_batch_4_functions.c | batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0041b520` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0041b6b0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0041ba90` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0041c4d0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0041c5a0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0041c720` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0041c950` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0041ccd0` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_0041d260` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0041d430` | navigator_batch_13_functions.c, navigator_batch_6_functions.c | batch6_functions.c, batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0041d5c0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0041d840` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0041d9c0` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0041e350` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0041e4c0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0041e5e0` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0041e7f0` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0041ea30` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0041ece0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0041eff0` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_0041f250` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0041fa10` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00420080` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_004202d0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_004205b0` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00420820` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_004208d0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00421690` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_00421d60` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00422030` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00422836` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00422dc0` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00422f60` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00423390` | navigator_batch_6_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00423710` | navigator_batch_6_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_004242c0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_004249f0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00424e40` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_004255e0` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_004259f0` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00425c70` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00426590` | navigator_batch_4_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00426900` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00426ed0` | navigator_batch_4_functions.c | batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_00427520` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00427750` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00428060` | navigator_batch_5_functions.c, navigator_batch_4_functions.c, navigator_batch_4_functions.c | - | Manual review required |
| `fcn_00428380` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_00429210` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00429260` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00429520` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00429710` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00429b70` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0042a000` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0042a620` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0042af00` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0042b120` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0042b460` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_0042bc90` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0042bfd0` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_0042c150` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0042d0f4` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0042d4c0` | navigator_batch_5_functions.c | batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_00437c83` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_004385e4` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0043cfc3` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0043d3cf` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0043dc7d` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0043dd4b` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0043de40` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0043e483` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0043e64e` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0043ee21` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_0043f91e` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0043f99a` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0043fcbd` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00440004` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0044070f` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00440adb` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00440c64` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0044164a` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_0044176e` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch3_functions.c, batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_00441bd1` | navigator_batch_13_functions.c, navigator_batch_4_functions.c | batch5_functions.c, batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_00441d30` | navigator_batch_13_functions.c, navigator_batch_4_functions.c | batch5_functions.c, batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_00441da4` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00441e8d` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_00441edd` | navigator_batch_13_functions.c, navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_004421a8` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_004421fd` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00442252` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00442a80` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00442c75` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00443678` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_004437dc` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00444d53` | navigator_batch_5_functions.c | batch4_functions.c | Keep navigator, exclude batch files |
| `fcn_00445f5f` | navigator_batch_5_functions.c | batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_00446000` | navigator_batch_4_functions.c | - | Manual review required |
| `fcn_0044bdbe` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0044bf62` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0044bfdd` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0044eb2d` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0044ef1a` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0044f0a8` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0044f263` | navigator_batch_5_functions.c | batch3_functions.c | Keep navigator, exclude batch files |
| `fcn_0044f383` | navigator_batch_4_functions.c, navigator_batch_5_functions.c | batch2_functions.c, batch2_functions.c | Keep navigator, exclude batch files |
| `fcn_0044f6a5` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00451a3f` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_00451b9b` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_00456390` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_0045664c` | navigator_batch_4_functions.c | batch5_functions.c | Keep navigator, exclude batch files |
| `fcn_0045738d` | navigator_batch_6_functions.c | batch6_functions.c | Keep navigator, exclude batch files |
| `fcn_004580af` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `fcn_0045a221` | navigator_batch_6_functions.c | batch7_functions.c | Keep navigator, exclude batch files |
| `game_main_loop` | navigator_batch_13_functions.c | - | Keep in main.c, remove from others |
| `loc_00406b50` | navigator_batch_5_functions.c, navigator_batch_4_functions.c, navigator_batch_4_functions.c | - | Manual review required |
| `memory_allocator` | navigator_batch_13_functions.c | - | Keep in main.c, remove from others |

## Summary

- **Total conflicts**: 170
- **Navigator vs Batch conflicts**: 147
- **Core function conflicts**: 5

## Resolution Strategy

1. **Exclude batch files** where navigator_batch_* variants exist
2. **Remove core symbols** from navigator files (keep in main.c/entry.c)
3. **Create canonical header** (sote_symbols.h) merging all function prototypes
4. **Update includes** to use canonical header
