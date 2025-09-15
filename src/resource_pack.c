#include "../include/sote.h"
#include "../include/resource_pack.h"
#ifdef SOTE_NATIVE_PACKS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
// Minimal zip reader via 'miniz' like approach is ideal; for now, shell out to 'unzip -p' if present.
static char g_pack_path[1024];
static char g_base_dir[1024];

static int have_cmd(const char* c){
    char buf[256]; snprintf(buf,sizeof(buf),"command -v %s >/dev/null 2>&1", c);
    return system(buf)==0;
}
int pack_open_for_level(int level, const char* base_dir){
    snprintf(g_base_dir,sizeof(g_base_dir), "%s", base_dir?base_dir:"assets_packs");
    snprintf(g_pack_path,sizeof(g_pack_path), "%s/data%02d.sotepak", g_base_dir, level);
    FILE* f = fopen(g_pack_path,"rb"); if(!f) return -1; fclose(f);
    return 0;
}
int pack_read_asset(const char* virt_path, void** out_buf, size_t* out_size){
    if(!have_cmd("unzip")) return -2;
    char cmd[1600];
    snprintf(cmd,sizeof(cmd),"unzip -p '%s' '%s' 2>/dev/null", g_pack_path, virt_path);
    FILE* p = popen(cmd,"r"); if(!p) return -3;
    size_t cap=1<<20, sz=0; unsigned char* buf=(unsigned char*)malloc(cap);
    if(!buf){ pclose(p); return -4; }
    for(;;){
        if(sz==cap){ cap*=2; buf=(unsigned char*)realloc(buf,cap); if(!buf){ pclose(p); return -4; } }
        size_t n=fread(buf+sz,1,1<<16,p); sz+=n; if(n==0) break;
    }
    pclose(p);
    *out_buf=buf; *out_size=sz; return 0;
}
void pack_close(void){ g_pack_path[0]=0; }
#endif