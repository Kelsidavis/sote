#pragma once
#include <stddef.h>
#include <stdint.h>
#include <SDL.h>

/* Load either:
 *  - true BMP (starts with 'BM'), pass-through to SDL_LoadBMP_RW()
 *  - DIB blob (starts with BITMAPINFO header size 40/56/108/124),
 *    we synthesize a 14-byte BITMAPFILEHEADER in-memory and feed SDL.
 * Returns a valid SDL_Surface* or NULL.
 */
SDL_Surface* LoadBMPLikeFromMemory(const void* bytes, size_t len);