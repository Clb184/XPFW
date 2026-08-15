#ifndef TEXTURE_INCLUDED
#define TEXTURE_INCLUDED

#include <GL/glew.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int width;
	int height;
	float texelw;
	float texelh;
} texture_metric_t;

typedef struct {
	GLuint texture; // Color texture
	GLuint depth; // Depth / Stencil texture
	GLuint framebuffer;

	// Keeping metrics just in case
	GLsizei width;
	GLsizei height;
} render_texture_t;

bool LoadTextureFromFile(const char* name, GLuint* tex_unit, texture_metric_t* metric); // Simply loads a texture from file, optionally get the size of it via pointers

bool LoadTextureFromMemory(char* data, GLuint* tex_unit, texture_metric_t* metric); // Load the texture from memory

bool CreateEmptyTexture(GLuint* tex_unit, int color); // Create a blank texture using a RGBA32 color per pixel

#define RTFLAG_DEPTH 0x0001

bool CreateRenderTexture(GLuint* tex_unit, GLuint* framebuffer, GLsizei width, GLsizei height); 

bool CreateRenderTextureA(render_texture_t* rt, GLsizei width, GLsizei height, int flags); 

int CheckRenderTexture(render_texture_t* rt); // Return 0 to see it exists

void DestroyRenderTexture(render_texture_t* rt);

#ifdef __cplusplus
}
#endif

#endif
