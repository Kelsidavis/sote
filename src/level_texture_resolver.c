#include "../include/sote.h"
#include "../include/video_gl.h"
#include "../include/vfs_packs.h"  /* existing VFS for .sotepak */
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int ci_ends_with(const char* s, const char* suf){
  size_t ls=strlen(s), lf=strlen(suf); if (lf>ls) return 0;
  for(size_t i=0;i<lf;i++){ char a=toupper((unsigned char)s[ls-lf+i]); char b=toupper((unsigned char)suf[i]); if(a!=b) return 0; }
  return 1;
}
static int ci_eq(const char* a, const char* b){
  for(;*a||*b; ++a,++b){ if(toupper((unsigned char)*a)!=toupper((unsigned char)*b)) return 0; } return 1;
}

/* Returns GL texture id or 0 on failure */
unsigned level_resolve_and_upload_texture(int level_id){
  const char* packdir = getenv("SOTE_DATA_PACK_DIR");
  if (!packdir){ fprintf(stderr,"[GFX] packdir missing; using fallback\n"); return 0; }

  char packname[64]; snprintf(packname,sizeof(packname),"data%02d.sotepak", level_id);

  /* Ensure VFS root is set - other parts of code might have changed it */
  vfs_packs_set_root(packdir);

  /* 1) Try exact candidates (common patterns) */
  const char* candidates[] = {
    "textures/embedded_000.bmp",
    "textures/embedded_001.bmp",
    "textures/level_%02d.bmp",
    "embedded_000.bmp",
    "level_%02d.bmp",
  };
  unsigned char* buf=NULL; size_t len=0;

  fprintf(stderr,"[GFX] level %d: searching pack %s in %s\n", level_id, packname, packdir);

  for (size_t i=0;i<sizeof(candidates)/sizeof(candidates[0]); ++i){
    char path[128];
    if (strstr(candidates[i], "%02d")) snprintf(path,sizeof(path),candidates[i], level_id);
    else snprintf(path,sizeof(path),"%s",candidates[i]);

    fprintf(stderr,"[GFX] level %d: trying '%s'\n", level_id, path);

    if (vfs_packs_read_file(packname, path, &buf, &len)==0 && buf && len>0){
      fprintf(stderr,"[GFX] level %d: found '%s', %zu bytes\n", level_id, path, len);
      SDL_RWops* rw = SDL_RWFromConstMem(buf,(int)len);
      SDL_Surface* s = rw ? SDL_LoadBMP_RW(rw,1) : NULL;
      if(!s){
        fprintf(stderr,"[GFX] level %d: found custom/encoded BMP '%s' - requires special decoder\n", level_id, path);
        free(buf); buf=NULL; len=0; continue;
      }
      unsigned tex = gl_upload_surface_rgba(s);
      if (tex) {
        SDL_FreeSurface(s);
        free(buf);
        fprintf(stderr,"[GFX] level %d: loaded exact '%s' (%zu bytes) -> tex %u\n", level_id, path, len, tex);
        return tex;
      } else {
        fprintf(stderr,"[GFX] level %d: failed to upload texture '%s'\n", level_id, path);
        SDL_FreeSurface(s);
        free(buf); buf=NULL; len=0; continue;
      }
    }
  }

  /* 2) Try some additional common patterns */
  const char* fallback_patterns[] = {
    "level.bmp",
    "background.bmp",
    "texture.bmp",
  };

  for (size_t i=0;i<sizeof(fallback_patterns)/sizeof(fallback_patterns[0]); ++i){
    fprintf(stderr,"[GFX] level %d: trying fallback '%s'\n", level_id, fallback_patterns[i]);
    if (vfs_packs_read_file(packname, fallback_patterns[i], &buf, &len)==0 && buf && len>0){
      SDL_RWops* rw = SDL_RWFromConstMem(buf,(int)len);
      SDL_Surface* s = rw ? SDL_LoadBMP_RW(rw,1) : NULL;
      if(!s){ free(buf); buf=NULL; len=0; continue; }
      unsigned tex = gl_upload_surface_rgba(s);
      if (tex) {
        SDL_FreeSurface(s);
        free(buf);
        fprintf(stderr,"[GFX] level %d: loaded fallback '%s' (%zu bytes) -> tex %u\n", level_id, fallback_patterns[i], len, tex);
        return tex;
      } else {
        SDL_FreeSurface(s);
        free(buf); buf=NULL; len=0; continue;
      }
    }
  }

  fprintf(stderr,"[GFX] level %d: no BMP found in pack; using fallback\n", level_id);
  return 0;
}