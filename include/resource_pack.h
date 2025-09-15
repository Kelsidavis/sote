#pragma once
#include "sote.h"
#ifdef SOTE_NATIVE_PACKS
int  pack_open_for_level(int level, const char* base_dir); // returns 0 on success
int  pack_read_asset(const char* virt_path, void** out_buf, size_t* out_size); // caller frees
void pack_close(void);
#endif