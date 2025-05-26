#ifndef SHADERPROGRAM_INCLUDED
#define SHADERPROGRAM_INCLUDED

#include <GLFW/glfw3.h>

namespace Clb184 {

	class CShaderProgram {
	public:
		void CreateProgram(GLuint vshader, GLuint fshader);
		void Bind();
	private:
		GLuint m_ProgramID;
	};

}

#endif