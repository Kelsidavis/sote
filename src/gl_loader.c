/*
 * RE-AGENT-BANNER
 * Agent: coordinator
 * Call ID: coord-gl-001
 * Artifact SHA256: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77
 * Provenance: OpenGL video path extension for SOTE
 */

#include "gl_loader.h"
#include <stdio.h>

// Global GL function table
GLFunctions g_gl = {0};

int gl_loader_init(void)
{
    if (g_gl.loaded) {
        return 1; // Already loaded
    }

    // Load all required OpenGL function pointers via SDL_GL_GetProcAddress
    g_gl.glClear = (void (*)(GLbitfield))SDL_GL_GetProcAddress("glClear");
    g_gl.glClearColor = (void (*)(GLfloat, GLfloat, GLfloat, GLfloat))SDL_GL_GetProcAddress("glClearColor");
    g_gl.glEnable = (void (*)(GLenum))SDL_GL_GetProcAddress("glEnable");
    g_gl.glDisable = (void (*)(GLenum))SDL_GL_GetProcAddress("glDisable");
    g_gl.glViewport = (void (*)(GLint, GLint, GLsizei, GLsizei))SDL_GL_GetProcAddress("glViewport");

    g_gl.glGenTextures = (void (*)(GLsizei, GLuint*))SDL_GL_GetProcAddress("glGenTextures");
    g_gl.glDeleteTextures = (void (*)(GLsizei, const GLuint*))SDL_GL_GetProcAddress("glDeleteTextures");
    g_gl.glBindTexture = (void (*)(GLenum, GLuint))SDL_GL_GetProcAddress("glBindTexture");
    g_gl.glTexImage2D = (void (*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))SDL_GL_GetProcAddress("glTexImage2D");
    g_gl.glTexParameteri = (void (*)(GLenum, GLenum, GLint))SDL_GL_GetProcAddress("glTexParameteri");

    g_gl.glBegin = (void (*)(GLenum))SDL_GL_GetProcAddress("glBegin");
    g_gl.glEnd = (void (*)(void))SDL_GL_GetProcAddress("glEnd");
    g_gl.glVertex2f = (void (*)(GLfloat, GLfloat))SDL_GL_GetProcAddress("glVertex2f");
    g_gl.glTexCoord2f = (void (*)(GLfloat, GLfloat))SDL_GL_GetProcAddress("glTexCoord2f");
    g_gl.glColor4f = (void (*)(GLfloat, GLfloat, GLfloat, GLfloat))SDL_GL_GetProcAddress("glColor4f");

    g_gl.glMatrixMode = (void (*)(GLenum))SDL_GL_GetProcAddress("glMatrixMode");
    g_gl.glLoadIdentity = (void (*)(void))SDL_GL_GetProcAddress("glLoadIdentity");
    g_gl.glOrtho = (void (*)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble))SDL_GL_GetProcAddress("glOrtho");

    g_gl.glGetString = (const GLubyte* (*)(GLenum))SDL_GL_GetProcAddress("glGetString");
    g_gl.glGetError = (GLenum (*)(void))SDL_GL_GetProcAddress("glGetError");

    // Check if all critical functions loaded
    if (!g_gl.glClear || !g_gl.glClearColor || !g_gl.glEnable || !g_gl.glDisable ||
        !g_gl.glViewport || !g_gl.glGenTextures || !g_gl.glDeleteTextures ||
        !g_gl.glBindTexture || !g_gl.glTexImage2D || !g_gl.glTexParameteri ||
        !g_gl.glBegin || !g_gl.glEnd || !g_gl.glVertex2f || !g_gl.glTexCoord2f || !g_gl.glColor4f ||
        !g_gl.glMatrixMode || !g_gl.glLoadIdentity || !g_gl.glOrtho ||
        !g_gl.glGetString || !g_gl.glGetError) {

        fprintf(stderr, "[GL_LOADER] ERROR: Failed to load one or more OpenGL functions\n");
        return 0;
    }

    g_gl.loaded = 1;

    // Log OpenGL version and renderer
    const GLubyte *version = GL_GET_STRING(GL_VERSION);
    const GLubyte *renderer = GL_GET_STRING(GL_RENDERER);
    const GLubyte *vendor = GL_GET_STRING(GL_VENDOR);

    fprintf(stderr, "[GL_LOADER] OpenGL loaded successfully\n");
    fprintf(stderr, "[GL_LOADER] Version: %s\n", version ? (const char*)version : "unknown");
    fprintf(stderr, "[GL_LOADER] Renderer: %s\n", renderer ? (const char*)renderer : "unknown");
    fprintf(stderr, "[GL_LOADER] Vendor: %s\n", vendor ? (const char*)vendor : "unknown");

    return 1;
}

void gl_loader_shutdown(void)
{
    // Clear function pointers
    g_gl.glClear = NULL;
    g_gl.glClearColor = NULL;
    g_gl.glEnable = NULL;
    g_gl.glDisable = NULL;
    g_gl.glViewport = NULL;
    g_gl.glGenTextures = NULL;
    g_gl.glDeleteTextures = NULL;
    g_gl.glBindTexture = NULL;
    g_gl.glTexImage2D = NULL;
    g_gl.glTexParameteri = NULL;
    g_gl.glBegin = NULL;
    g_gl.glEnd = NULL;
    g_gl.glVertex2f = NULL;
    g_gl.glTexCoord2f = NULL;
    g_gl.glColor4f = NULL;
    g_gl.glMatrixMode = NULL;
    g_gl.glLoadIdentity = NULL;
    g_gl.glOrtho = NULL;
    g_gl.glGetString = NULL;
    g_gl.glGetError = NULL;

    g_gl.loaded = 0;

    fprintf(stderr, "[GL_LOADER] OpenGL function pointers cleared\n");
}