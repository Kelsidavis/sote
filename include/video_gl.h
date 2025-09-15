#pragma once
#include <SDL.h>

unsigned gl_upload_surface_rgba(SDL_Surface* s);
void gl_draw_fullscreen(unsigned tex);
unsigned level_resolve_and_upload_texture(int level_id);

extern unsigned g_level_tex_override;