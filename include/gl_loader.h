#ifndef GL_LOADER_H
#define GL_LOADER_H

/*
 * RE-AGENT-BANNER
 * Agent: coordinator
 * Call ID: coord-gl-001
 * Artifact SHA256: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77
 * Provenance: OpenGL video path extension for SOTE
 */

#include "SDL_compat.h"

#ifdef WIN32_BUILD
#include <windows.h>
#include <GL/gl.h>
#else
#include <GL/gl.h>
#endif

// OpenGL function pointers loaded dynamically via SDL_GL_GetProcAddress
typedef struct {
    // Core OpenGL functions needed for textured blitting
    void (*glClear)(GLbitfield mask);
    void (*glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    void (*glEnable)(GLenum cap);
    void (*glDisable)(GLenum cap);
    void (*glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

    // Texture functions
    void (*glGenTextures)(GLsizei n, GLuint *textures);
    void (*glDeleteTextures)(GLsizei n, const GLuint *textures);
    void (*glBindTexture)(GLenum target, GLuint texture);
    void (*glTexImage2D)(GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLint border,
                         GLenum format, GLenum type, const GLvoid *pixels);
    void (*glTexParameteri)(GLenum target, GLenum pname, GLint param);

    // Quad rendering functions
    void (*glBegin)(GLenum mode);
    void (*glEnd)(void);
    void (*glVertex2f)(GLfloat x, GLfloat y);
    void (*glTexCoord2f)(GLfloat s, GLfloat t);
    void (*glColor4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

    // Matrix functions
    void (*glMatrixMode)(GLenum mode);
    void (*glLoadIdentity)(void);
    void (*glOrtho)(GLdouble left, GLdouble right, GLdouble bottom,
                    GLdouble top, GLdouble zNear, GLdouble zFar);

    // State query
    const GLubyte* (*glGetString)(GLenum name);
    GLenum (*glGetError)(void);

    int loaded; // 1 if all functions loaded successfully
} GLFunctions;

// Global GL function table
extern GLFunctions g_gl;

// Initialize OpenGL function loading via SDL
// Returns 1 on success, 0 on failure
int gl_loader_init(void);

// Cleanup GL function pointers
void gl_loader_shutdown(void);

// Convenience macros for GL calls
#define GL_CALL_CLEAR(mask) do { if(g_gl.glClear) g_gl.glClear(mask); } while(0)
#define GL_CALL_CLEAR_COLOR(r,g,b,a) do { if(g_gl.glClearColor) g_gl.glClearColor(r,g,b,a); } while(0)
#define GL_CALL_ENABLE(cap) do { if(g_gl.glEnable) g_gl.glEnable(cap); } while(0)
#define GL_CALL_DISABLE(cap) do { if(g_gl.glDisable) g_gl.glDisable(cap); } while(0)
#define GL_CALL_VIEWPORT(x,y,w,h) do { if(g_gl.glViewport) g_gl.glViewport(x,y,w,h); } while(0)

#define GL_GEN_TEXTURES(n,tex) do { if(g_gl.glGenTextures) g_gl.glGenTextures(n,tex); } while(0)
#define GL_DELETE_TEXTURES(n,tex) do { if(g_gl.glDeleteTextures) g_gl.glDeleteTextures(n,tex); } while(0)
#define GL_BIND_TEXTURE(target,tex) do { if(g_gl.glBindTexture) g_gl.glBindTexture(target,tex); } while(0)
#define GL_TEX_IMAGE_2D(target,level,internal,w,h,border,format,type,data) \
    do { if(g_gl.glTexImage2D) g_gl.glTexImage2D(target,level,internal,w,h,border,format,type,data); } while(0)
#define GL_TEX_PARAMETER_I(target,pname,param) \
    do { if(g_gl.glTexParameteri) g_gl.glTexParameteri(target,pname,param); } while(0)

#define GL_BEGIN(mode) do { if(g_gl.glBegin) g_gl.glBegin(mode); } while(0)
#define GL_END() do { if(g_gl.glEnd) g_gl.glEnd(); } while(0)
#define GL_VERTEX_2F(x,y) do { if(g_gl.glVertex2f) g_gl.glVertex2f(x,y); } while(0)
#define GL_TEX_COORD_2F(s,t) do { if(g_gl.glTexCoord2f) g_gl.glTexCoord2f(s,t); } while(0)
#define GL_COLOR_4F(r,g,b,a) do { if(g_gl.glColor4f) g_gl.glColor4f(r,g,b,a); } while(0)

#define GL_CALL_MATRIX_MODE(mode) do { if(g_gl.glMatrixMode) g_gl.glMatrixMode(mode); } while(0)
#define GL_CALL_LOAD_IDENTITY() do { if(g_gl.glLoadIdentity) g_gl.glLoadIdentity(); } while(0)
#define GL_CALL_ORTHO(l,r,b,t,n,f) do { if(g_gl.glOrtho) g_gl.glOrtho(l,r,b,t,n,f); } while(0)

#define GL_GET_STRING(name) (g_gl.glGetString ? g_gl.glGetString(name) : (const GLubyte*)"N/A")
#define GL_GET_ERROR() (g_gl.glGetError ? g_gl.glGetError() : GL_NO_ERROR)

#endif // GL_LOADER_H