#include "OpenGL/CShaderFactory.hpp"
#include "Output.hpp"

namespace Clb184 {

	constexpr int INFOLOG_LENGTH = 1024;

	CShaderFactory::CShaderFactory() {

	}

	CShaderFactory::~CShaderFactory() {

	}

	GLuint CShaderFactory::CreateShader(GLenum shader_type, const GLchar* src, GLsizei size) {

		LOG_INFO("Creating shader"); 
		GLuint shader = glCreateShader(shader_type);
		GLint status = -1;
		GLchar buffer[INFOLOG_LENGTH] = "";

		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (0 == status) {
			glGetShaderInfoLog(shader, INFOLOG_LENGTH, NULL, buffer);
			LOG_ERROR(buffer);
			return -1;
		}

		return shader;
	}

}