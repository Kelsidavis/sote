#pragma once
#include <stddef.h>
#include <SDL.h>

void vfs_packs_set_root(const char* dir);
void* vfs_packs_slurp(const char* pack_name, const char* inner_path, size_t* out_sz);
SDL_RWops* vfs_packs_open_rw(const char* pack_name, const char* inner_path);