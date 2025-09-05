#ifndef SHADER_INCLUDED
#define SHADER_INCLUDED

#include <GL/glew.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool LoadShaderFromFile(const char* file_name, GLuint* shader_unit, GLenum shader_type);
bool LoadShaderFromMemory(const char* src, GLuint* shader_unit, GLenum shader_type);
bool CreateShaderProgram(GLuint vertex_shader, GLuint fragment_shader, GLuint* program_unit);

#ifdef __cplusplus
}
#endif

#endif