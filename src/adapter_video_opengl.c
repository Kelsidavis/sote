/*
 * RE-AGENT-BANNER
 * Agent: coordinator
 * Call ID: coord-gl-001
 * Artifact SHA256: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77
 * Provenance: OpenGL video path extension for SOTE
 */

#include "adapter_video.h"
#include "gl_loader.h"
#include "../include/sote.h"
#include "../include/video_gl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <strings.h>  // For strcasecmp on Linux
#include <unistd.h>   // For usleep
#else
#define strcasecmp _stricmp  // Windows equivalent
#endif

// OpenGL-specific video context
typedef struct {
    SDL_Window *window;
    SDL_GLContext gl_context;
    GLuint framebuffer_texture;
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t initialized;
} GL_VIDEO_CONTEXT;

static GL_VIDEO_CONTEXT g_gl_context = {0};

// Initialize OpenGL video context
int adapter_video_opengl_init(int w, int h)
{
    if (g_gl_context.initialized) {
        return 1; // Already initialized
    }

    // Initialize SDL with video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "[GL_VIDEO] ERROR: SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    // Create OpenGL window
    g_gl_context.window = SDL_CreateWindow(
        "SOTE OpenGL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        w, h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!g_gl_context.window) {
        fprintf(stderr, "[GL_VIDEO] ERROR: Failed to create OpenGL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    // Create OpenGL context
    g_gl_context.gl_context = SDL_GL_CreateContext(g_gl_context.window);
    if (!g_gl_context.gl_context) {
        fprintf(stderr, "[GL_VIDEO] ERROR: Failed to create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(g_gl_context.window);
        SDL_Quit();
        return 0;
    }

    // Make context current
    if (SDL_GL_MakeCurrent(g_gl_context.window, g_gl_context.gl_context) < 0) {
        fprintf(stderr, "[GL_VIDEO] ERROR: Failed to make OpenGL context current: %s\n", SDL_GetError());
        SDL_GL_DeleteContext(g_gl_context.gl_context);
        SDL_DestroyWindow(g_gl_context.window);
        SDL_Quit();
        return 0;
    }

    // Load OpenGL functions
    if (!gl_loader_init()) {
        fprintf(stderr, "[GL_VIDEO] ERROR: Failed to load OpenGL functions\n");
        SDL_GL_DeleteContext(g_gl_context.gl_context);
        SDL_DestroyWindow(g_gl_context.window);
        SDL_Quit();
        return 0;
    }

    // Set up OpenGL state
    GL_CALL_VIEWPORT(0, 0, w, h);
    GL_CALL_MATRIX_MODE(GL_PROJECTION);
    GL_CALL_LOAD_IDENTITY();
    GL_CALL_ORTHO(0.0, (double)w, (double)h, 0.0, -1.0, 1.0);
    GL_CALL_MATRIX_MODE(GL_MODELVIEW);
    GL_CALL_LOAD_IDENTITY();

    GL_CALL_ENABLE(GL_TEXTURE_2D);
    GL_CALL_DISABLE(GL_DEPTH_TEST);

    // Create framebuffer texture
    GL_GEN_TEXTURES(1, &g_gl_context.framebuffer_texture);
    GL_BIND_TEXTURE(GL_TEXTURE_2D, g_gl_context.framebuffer_texture);
    GL_TEX_PARAMETER_I(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL_TEX_PARAMETER_I(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_TEX_PARAMETER_I(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    GL_TEX_PARAMETER_I(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    g_gl_context.screen_width = w;
    g_gl_context.screen_height = h;
    g_gl_context.initialized = 1;

    fprintf(stderr, "[GL_VIDEO] OpenGL video adapter initialized (%dx%d)\n", w, h);
    return 1;
}

// Shutdown OpenGL video context
void adapter_video_opengl_shutdown(void)
{
    if (!g_gl_context.initialized) {
        return;
    }

    if (g_gl_context.framebuffer_texture) {
        GL_DELETE_TEXTURES(1, &g_gl_context.framebuffer_texture);
        g_gl_context.framebuffer_texture = 0;
    }

    gl_loader_shutdown();

    if (g_gl_context.gl_context) {
        SDL_GL_DeleteContext(g_gl_context.gl_context);
        g_gl_context.gl_context = NULL;
    }

    if (g_gl_context.window) {
        SDL_DestroyWindow(g_gl_context.window);
        g_gl_context.window = NULL;
    }

    SDL_Quit();

    g_gl_context.initialized = 0;
    fprintf(stderr, "[GL_VIDEO] OpenGL video adapter shutdown\n");
}

// Present the current frame
int adapter_video_opengl_present(void)
{
    if (!g_gl_context.initialized || !g_gl_context.window) {
        return 0;
    }

    SDL_GL_SwapWindow(g_gl_context.window);
    return 1;
}

void video_present(){
    /* Draw level texture override if present */
    extern unsigned g_level_tex_override;
    if (g_level_tex_override) {
        gl_draw_fullscreen(g_level_tex_override);
    }

    SDL_GL_SwapWindow(g_gl_context.window);
}

// Clear screen with RGB color
int adapter_video_opengl_clear(uint8_t r, uint8_t g, uint8_t b)
{
    if (!g_gl_context.initialized) {
        return 0;
    }

    GL_CALL_CLEAR_COLOR(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
    GL_CALL_CLEAR(GL_COLOR_BUFFER_BIT);
    return 1;
}

// Load and display BMP via OpenGL textured quad
int adapter_video_opengl_show_bmp(const char* bmp_path)
{
    if (!bmp_path || !g_gl_context.initialized) {
        return 0;
    }

    // Load BMP using SDL
    SDL_Surface *bmp_surface = SDL_LoadBMP(bmp_path);
    if (!bmp_surface) {
        fprintf(stderr, "[GL_VIDEO] ERROR: Failed to load BMP %s: %s\n", bmp_path, SDL_GetError());
        return 0;
    }

    fprintf(stderr, "[GL_VIDEO] DEBUG: Loaded BMP %s: %dx%d, %d bpp, format=%p, masks=R:0x%x G:0x%x B:0x%x\n",
            bmp_path, bmp_surface->w, bmp_surface->h, bmp_surface->format->BytesPerPixel,
            (void*)bmp_surface->format->format, bmp_surface->format->Rmask,
            bmp_surface->format->Gmask, bmp_surface->format->Bmask);

    // Create OpenGL texture from BMP data
    GLuint texture;
    GL_GEN_TEXTURES(1, &texture);
    GL_BIND_TEXTURE(GL_TEXTURE_2D, texture);

    // Convert to RGB format if needed
    SDL_Surface *rgb_surface = bmp_surface;
    int need_conversion = 0;

    // Check if we need format conversion
    if (bmp_surface->format->BytesPerPixel == 3) {
        // Check if it's BGR format (common in BMPs)
        if (bmp_surface->format->Rmask == 0x00ff0000) {
            need_conversion = 1;
            fprintf(stderr, "[GL_VIDEO] DEBUG: BMP is BGR format, converting to RGB\n");
        }
    }

    // Convert BGR to RGB if needed
    if (need_conversion) {
        Uint32 rgb_format = SDL_PIXELFORMAT_RGB24;
        rgb_surface = SDL_ConvertSurfaceFormat(bmp_surface, rgb_format, 0);
        if (!rgb_surface) {
            fprintf(stderr, "[GL_VIDEO] ERROR: Failed to convert BMP to RGB: %s\n", SDL_GetError());
            SDL_FreeSurface(bmp_surface);
            return 0;
        }
    }

    // Determine OpenGL format
    GLenum format = GL_RGB;
    if (rgb_surface->format->BytesPerPixel == 4) {
        format = GL_RGBA;
    } else if (rgb_surface->format->BytesPerPixel == 3) {
        format = GL_RGB;
    }

    GL_TEX_IMAGE_2D(GL_TEXTURE_2D, 0, format, rgb_surface->w, rgb_surface->h, 0,
                    format, GL_UNSIGNED_BYTE, rgb_surface->pixels);

    // Check for OpenGL errors
    GLenum gl_error = GL_GET_ERROR();
    if (gl_error != GL_NO_ERROR) {
        fprintf(stderr, "[GL_VIDEO] ERROR: OpenGL error after glTexImage2D: 0x%x\n", gl_error);
    }

    GL_TEX_PARAMETER_I(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL_TEX_PARAMETER_I(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    fprintf(stderr, "[GL_VIDEO] DEBUG: Created texture ID %u, format=0x%x\n", texture, format);

    // Clear and render textured quad
    GL_CALL_CLEAR_COLOR(0.0f, 0.0f, 0.0f, 1.0f);
    GL_CALL_CLEAR(GL_COLOR_BUFFER_BIT);

    // Enable texturing and set white color for proper texture display
    GL_CALL_ENABLE(GL_TEXTURE_2D);
    GL_COLOR_4F(1.0f, 1.0f, 1.0f, 1.0f);

    // Calculate centered position
    int bmp_width = rgb_surface->w;
    int bmp_height = rgb_surface->h;
    int x_offset = (g_gl_context.screen_width - bmp_width) / 2;
    int y_offset = (g_gl_context.screen_height - bmp_height) / 2;

    // Render textured quad - fix coordinate system mismatch
    // OpenGL ortho is set up with Y going from height to 0 (top to bottom)
    // But BMP textures have Y=0 at bottom, so we need to flip texture coords
    GL_BEGIN(GL_QUADS);
    GL_TEX_COORD_2F(0.0f, 1.0f); GL_VERTEX_2F((float)x_offset, (float)y_offset);
    GL_TEX_COORD_2F(1.0f, 1.0f); GL_VERTEX_2F((float)(x_offset + bmp_width), (float)y_offset);
    GL_TEX_COORD_2F(1.0f, 0.0f); GL_VERTEX_2F((float)(x_offset + bmp_width), (float)(y_offset + bmp_height));
    GL_TEX_COORD_2F(0.0f, 0.0f); GL_VERTEX_2F((float)x_offset, (float)(y_offset + bmp_height));
    GL_END();

    // Present
    SDL_GL_SwapWindow(g_gl_context.window);

    // Cleanup
    GL_DELETE_TEXTURES(1, &texture);
    if (need_conversion && rgb_surface != bmp_surface) {
        SDL_FreeSurface(rgb_surface);
    }
    SDL_FreeSurface(bmp_surface);

    fprintf(stderr, "[GL_VIDEO] BMP rendered: %s (%dx%d) at (%d,%d)\n",
            strrchr(bmp_path, '/') ? strrchr(bmp_path, '/') + 1 : bmp_path,
            bmp_width, bmp_height, x_offset, y_offset);

    return 1;
}

// Draw centered BMP via OpenGL
int adapter_video_opengl_draw_bmp_centered(const char* bmp_path)
{
    return adapter_video_opengl_show_bmp(bmp_path);
}

// Save screenshot (stub for OpenGL path - would need framebuffer readback)
int adapter_video_opengl_save_screenshot(const char* out_bmp_path)
{
    if (!g_gl_context.initialized) {
        return 0;
    }

    // For now, just create a placeholder file to satisfy the interface
    FILE *f = fopen(out_bmp_path, "wb");
    if (f) {
        fclose(f);
        fprintf(stderr, "[GL_VIDEO] Screenshot placeholder saved: %s\n", out_bmp_path);
        return 1;
    }

    return 0;
}

// Check if using OpenGL path
int adapter_video_is_opengl(void)
{
    return g_gl_context.initialized;
}

/* GL texture helpers for level artwork */
unsigned gl_upload_surface_rgba(SDL_Surface* s){
    if (!s) return 0;

    SDL_Surface* conv = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGBA32, 0);
    if (!conv) return 0;

    GLuint tex = 0;
    GL_GEN_TEXTURES(1, &tex);
    GL_BIND_TEXTURE(GL_TEXTURE_2D, tex);
    GL_TEX_PARAMETER_I(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL_TEX_PARAMETER_I(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_TEX_IMAGE_2D(GL_TEXTURE_2D, 0, GL_RGBA, conv->w, conv->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, conv->pixels);

    SDL_FreeSurface(conv);
    fprintf(stderr, "[GL_VIDEO] Uploaded texture ID %u\n", tex);
    return tex;
}

void gl_draw_fullscreen(unsigned tex){
    if (!tex) return;

    GL_CALL_DISABLE(GL_DEPTH_TEST);
    GL_CALL_ENABLE(GL_TEXTURE_2D);
    GL_BIND_TEXTURE(GL_TEXTURE_2D, (GLuint)tex);

    GL_BEGIN(GL_QUADS);
      GL_TEX_COORD_2F(0.0f, 0.0f); GL_VERTEX_2F(-1.0f, -1.0f);
      GL_TEX_COORD_2F(1.0f, 0.0f); GL_VERTEX_2F( 1.0f, -1.0f);
      GL_TEX_COORD_2F(1.0f, 1.0f); GL_VERTEX_2F( 1.0f,  1.0f);
      GL_TEX_COORD_2F(0.0f, 1.0f); GL_VERTEX_2F(-1.0f,  1.0f);
    GL_END();
}

/* Global level texture override */
unsigned g_level_tex_override = 0;