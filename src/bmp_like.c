#include "SDL_compat.h"
#include "../include/bmp_like.h"
#include <string.h>
#include <stdlib.h>

static int is_bmp(const uint8_t* p, size_t n){ return n>=2 && p[0]=='B' && p[1]=='M'; }
static int is_dib_header_size(uint32_t sz){
  return (sz==40u || sz==56u || sz==108u || sz==124u);
}
SDL_Surface* LoadBMPLikeFromMemory(const void* bytes, size_t len){
  if(!bytes || len<4) return NULL;
  const uint8_t* p = (const uint8_t*)bytes;

  /* True BMP? Let SDL try it first, but don't give up if it fails. */
  if(is_bmp(p, len)){
    SDL_RWops* rw = SDL_RWFromConstMem(bytes, (int)len);
    if(rw) {
      SDL_Surface* s = SDL_LoadBMP_RW(rw, 1);
      if(s) return s;
      /* SDL failed on a BMP - might be corrupted, try DIB recovery below */
    }
  }

  /* Maybe a DIB: expect little-endian header size at start */
  if(len>=4){
    uint32_t hdr = (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
    if(is_dib_header_size(hdr)){
      /* Synthesize a BITMAPFILEHEADER (14 bytes) + blob */
      const uint32_t bfOffBits = 14u + hdr; /* file header + info header; color table, if present, is already in the blob after info header */
      uint32_t bfSize = (uint32_t)(14u + len);
      uint8_t head[14];
      head[0]='B'; head[1]='M';
      head[2]=(uint8_t)(bfSize&0xFF); head[3]=(uint8_t)((bfSize>>8)&0xFF);
      head[4]=(uint8_t)((bfSize>>16)&0xFF); head[5]=(uint8_t)((bfSize>>24)&0xFF);
      head[6]=head[7]=head[8]=head[9]=0; /* bfReserved1/2 */
      head[10]=(uint8_t)(bfOffBits&0xFF);
      head[11]=(uint8_t)((bfOffBits>>8)&0xFF);
      head[12]=(uint8_t)((bfOffBits>>16)&0xFF);
      head[13]=(uint8_t)((bfOffBits>>24)&0xFF);

      /* Allocate contiguous memory: [14-byte header][DIB blob] */
      void* buf = SDL_malloc(14 + len);
      if(!buf) return NULL;
      memcpy((uint8_t*)buf, head, 14);
      memcpy((uint8_t*)buf + 14, p, len);

      SDL_RWops* rw = SDL_RWFromMem(buf, (int)(14 + len));
      if(!rw){ SDL_free(buf); return NULL; }
      /* SDL_LoadBMP_RW will free RW but not your buffer; so close manually after load */
      SDL_Surface* s = SDL_LoadBMP_RW(rw, 0);
      SDL_RWclose(rw);
      if(!s){ SDL_free(buf); return NULL; }

      /* Attach a cleanup hook by replacing pixel free? Easiest: stash pointer in surface's userdata (SDL2 lacks it). */
      /* We'll just leak on error paths avoided above; to be safe, copy surface to a fresh one and free buf */
      SDL_Surface* cpy = SDL_ConvertSurfaceFormat(s, s->format->format, 0);
      SDL_FreeSurface(s);
      SDL_free(buf);
      return cpy;
    }
  }

  /* If we've reached here, the file might be corrupted or use a proprietary format.
   * For "embedded_*.bmp" files that seem corrupted, we might need special handling.
   * For now, just give up. */
  return NULL;
}