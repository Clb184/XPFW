#include "OpenGL/CShaderProgram.hpp"
#include "Output.hpp"

namespace Clb184 {

	CShaderProgram::CShaderProgram() {

		m_ProgramID = -1;
		m_VShaderID = -1;
		m_FShaderID = -1;
	}

	CShaderProgram::~CShaderProgram() {
		if (GL_TRUE == glIsProgram(m_ProgramID)) {
			glUseProgram(0);
			glDeleteProgram(m_ProgramID);
		}
	}

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

	void CShaderProgram::Bind() const {
		glUseProgram(m_ProgramID);
	}

	GLuint CShaderProgram::GetVertexShader() const {
		return m_VShaderID;
	}

	GLuint CShaderProgram::GetFragmentShader() const {
		return m_FShaderID;
	}

	GLuint CShaderProgram::GetProgram() const {
		return m_ProgramID;
	}
}