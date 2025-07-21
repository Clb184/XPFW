#ifndef SHADER_INCLUDED
#define SHADER_INCLUDED

#include <GL/glew.h>

namespace Clb184 {
	bool LoadShaderFromFile(const char* file_name, GLuint* shader_unit, GLenum shader_type);
	bool LoadShaderFromMemory(const char* src, GLuint* shader_unit, GLenum shader_type);
	bool CreateProgram(GLuint vertex_shader, GLuint fragment_shader, GLuint* program_unit);
}

#endif