#include "OpenGL/Shader.hpp"
#include <assert.h>
#include "Output.hpp"
#include "IO.hpp"

bool LoadShaderFromFile(const char* file_name, GLuint* shader_unit, GLenum shader_type) {
	LOG_INFO("Creating Shader (File)");
	assert(nullptr != shader_unit);

	char* src = nullptr;
	if(false == LoadTextFromFile(file_name, &src, nullptr)) return false;

	GLuint shader = glCreateShader(shader_type);
	GLint status = -1;
	GLchar buffer[1024] = "";

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (0 == status) {
		glGetShaderInfoLog(shader, 1024, NULL, buffer);
		fprintf(stderr, "Shader compilation error log:\n%s\n", buffer);
	}

	GL_ERROR_RETURN();
	free(src);
	*shader_unit = shader;
	return true;
}

bool LoadShaderFromMemory(const char* src, GLuint* shader_unit, GLenum shader_type) {
	LOG_INFO("Creating Shader (Memory)");

	assert(nullptr != src); 
	assert(nullptr != shader_unit);

	GLuint shader = glCreateShader(shader_type);
	GLint status = -1;
	GLchar buffer[1024] = "";

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (0 == status) {
		glGetShaderInfoLog(shader, 1024, NULL, buffer);
		fprintf(stderr, "Shader compilation error log:\n%s\n", buffer);
	}

	GL_ERROR_RETURN();
	*shader_unit = shader;
	return true;
}

bool CreateShaderProgram(GLuint vertex_shader, GLuint fragment_shader, GLuint* program_unit) {
	LOG_INFO("Creating Shader Program");

	assert(nullptr != program_unit);

	GLuint program = glCreateProgram();
	GLint status = 0;
	char buffer[1024] = "";

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (0 == status) {
		glGetProgramInfoLog(program, 1024, NULL, buffer);
		fprintf(stderr, "Program compilation error log:\n%s\n", buffer);
	}

	GL_ERROR_RETURN();
	*program_unit = program;
	return true;
}