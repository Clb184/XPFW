#ifndef CSHADERFACTORY_INCLUDED
#define CSHADERFACTORY_INCLUDED

#include <GL/glew.h>
#include <stdint.h>

namespace Clb184 {
	class CShaderFactory {
	public:
		CShaderFactory();
		~CShaderFactory();

		GLuint CreateShader(GLenum shader_type, const GLchar* src, GLsizei size);

	private:
		uint32_t m_NumShadersCreated = 0;
	};
}

#endif