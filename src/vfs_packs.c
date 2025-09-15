#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include "../include/sote.h"
#include "../include/vfs_packs.h"

static char s_pack_dir[1024] = {0};

void vfs_packs_set_root(const char* dir){
    if(dir) snprintf(s_pack_dir, sizeof(s_pack_dir), "%s", dir);
}

/* returns malloc'd memory + size, or NULL */
void* vfs_packs_slurp(const char* pack_name, const char* inner_path, size_t* out_sz){
    if(!s_pack_dir[0] || !pack_name || !inner_path) return NULL;

    char zip_path[1024];
    snprintf(zip_path, sizeof(zip_path), "%s/%s", s_pack_dir, pack_name);

    // Use system unzip to extract to stdout
    char cmd[2048];
    snprintf(cmd, sizeof(cmd), "unzip -p '%s' '%s' 2>/dev/null", zip_path, inner_path);

    FILE* p = popen(cmd, "r");
    if(!p) return NULL;

    size_t cap = 1024*1024, sz = 0;
    unsigned char* buf = malloc(cap);
    if(!buf) { pclose(p); return NULL; }

    for(;;) {
        if(sz == cap) {
            cap *= 2;
            buf = realloc(buf, cap);
            if(!buf) { pclose(p); return NULL; }
        }
        size_t n = fread(buf + sz, 1, 4096, p);
        sz += n;
        if(n == 0) break;
    }
    pclose(p);

    if(sz == 0) { free(buf); return NULL; }
    *out_sz = sz;
    return buf;
}

SDL_RWops* vfs_packs_open_rw(const char* pack_name, const char* inner_path){
    size_t sz = 0;
    void* mem = vfs_packs_slurp(pack_name, inner_path, &sz);
    if(!mem) return NULL;

    SDL_RWops* rw = SDL_RWFromConstMem(mem, (int)sz);
    // Note: this leaks mem, but SDL_RWFromConstMem doesn't own the memory
    // For production, would need custom RWops with cleanup
    return rw;
}