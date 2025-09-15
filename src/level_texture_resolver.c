#include <stdio.h>
#include <SDL.h>
#include "../include/sote.h"
#include "../include/vfs_packs.h"
#include "../include/video_gl.h"

static const char* k_patterns[] = {
  "textures/embedded_000.bmp",
  "textures/embedded_001.bmp",
  "textures/level.bmp",
  "data/level.bmp",
  "embedded_000.bmp",
  "embedded_001.bmp"
};

/* returns GL texture id (>0) or 0 on failure; logs what it loads */
unsigned level_resolve_and_upload_texture(int level_id){
  char pack[64];
  snprintf(pack, sizeof(pack), "data%02d.sotepak", level_id);

  for (size_t i = 0; i < sizeof(k_patterns)/sizeof(k_patterns[0]); ++i){
    const char* inner = k_patterns[i];
    SDL_RWops* rw = vfs_packs_open_rw(pack, inner);
    if (!rw) continue;

    SDL_Surface* surf = SDL_LoadBMP_RW(rw, 1);
    if (!surf) continue;

    unsigned tex = gl_upload_surface_rgba(surf);
    if (tex){
      fprintf(stderr, "[GFX] level %d: loaded '%s' from %s (w=%d h=%d)\n",
              level_id, inner, pack, surf->w, surf->h);
      SDL_FreeSurface(surf);
      return tex;
    }
    SDL_FreeSurface(surf);
  }

  fprintf(stderr, "[GFX] level %d: no pack BMP found; will use fallback\n", level_id);
  return 0;
}