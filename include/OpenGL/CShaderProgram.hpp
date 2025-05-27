#ifndef SHADERPROGRAM_INCLUDED
#define SHADERPROGRAM_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Clb184 {

	class CShaderProgram {
	public:
		bool CreateProgram(GLuint vshader, GLuint fshader);
		void Bind();
	private:
		GLuint m_ProgramID;
		GLuint m_VShaderID;
		GLuint m_FShaderID;
	};

}

#endif