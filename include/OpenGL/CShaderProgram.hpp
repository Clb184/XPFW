#ifndef SHADERPROGRAM_INCLUDED
#define SHADERPROGRAM_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Clb184 {

	class CShaderProgram {
	public:
		CShaderProgram();
		~CShaderProgram();

		bool CreateProgram(GLuint vshader, GLuint fshader);
		void Bind() const;
		GLuint GetVertexShader() const;
		GLuint GetFragmentShader() const;
		GLuint GetProgram() const;

	private:
		GLuint m_ProgramID;
		GLuint m_VShaderID;
		GLuint m_FShaderID;
	};

}

#endif