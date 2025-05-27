#include "OpenGL/CShaderProgram.hpp"
#include "Output.hpp"

namespace Clb184 {

	bool CShaderProgram::CreateProgram(GLuint vshader, GLuint fshader) {
		LOG_INFO("Creating Program");
		GLuint program = glCreateProgram();
		glAttachShader(program, vshader);
		glAttachShader(program, fshader);
		glLinkProgram(program);

		GLint status = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		char buffer[1024] = "";
		if (0 == status) {
			glGetProgramInfoLog(program, 1024, NULL, buffer);
			LOG_ERROR(buffer);
			return false;
		}
		m_ProgramID = program;
		m_VShaderID = vshader;
		m_FShaderID = fshader;
		return true;
	}

}