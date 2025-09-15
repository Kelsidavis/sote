#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <SDL.h>
#include "../include/sote.h"
#include "../include/vfs_packs.h"
#include "../include/video_gl.h"

/* Returns GL texture id (>0) or 0 on failure; logs what it loads */
unsigned level_resolve_and_upload_texture(int level_id){
    char pack[64];
    snprintf(pack, sizeof(pack), "data%02d.sotepak", level_id);

    const char* candidates[] = {
        "textures/embedded_000.bmp",
        "textures/embedded_001.bmp",
        "embedded_000.bmp",
        "embedded_001.bmp",
        "level.bmp"
    };

    for (size_t i = 0; i < sizeof(candidates)/sizeof(candidates[0]); ++i){
        const char* inner = candidates[i];

        unsigned char* buf = NULL;
        size_t len = 0;

        if (vfs_packs_read_file(pack, inner, &buf, &len) == 0 && buf && len > 0) {
            SDL_RWops* rw = SDL_RWFromConstMem(buf, (int)len);
            if (!rw) { free(buf); continue; }

            SDL_Surface* surf = SDL_LoadBMP_RW(rw, 1);
            if (!surf) { free(buf); continue; }

            unsigned tex = gl_upload_surface_rgba(surf);
            if (tex){
                fprintf(stderr, "[GFX] level %d: loaded '%s' from %s (w=%d h=%d, %zu bytes)\n",
                        level_id, inner, pack, surf->w, surf->h, len);
                SDL_FreeSurface(surf);
                free(buf);
                return tex;
            }
            SDL_FreeSurface(surf);
            free(buf);
        }
    }

    fprintf(stderr, "[GFX] level %d: no pack BMP found; will use fallback\n", level_id);
    return 0;
}