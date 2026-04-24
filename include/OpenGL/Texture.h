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

bool LoadTextureFromFile(const char* name, GLuint* tex_unit, texture_metric_t* metric); // Simply loads a texture from file, optionally get the size of it via pointers

bool LoadTextureFromMemory(char* data, GLuint* tex_unit, texture_metric_t* metric); // Load the texture from memory

bool CreateEmptyTexture(GLuint* tex_unit, int color); // Create a blank texture using a RGBA32 color per pixel

bool CreateRenderTexture(GLuint* tex_unit, GLuint* framebuffer, GLsizei width, GLsizei height); 

#ifdef __cplusplus
}
#endif

#endif
