#ifndef VERTEXATTRIBUTE_INCLUDED
#define VERTEXATTRIBUTE_INCLUDED

#include <GL/glew.h>

namespace Clb184 {

	struct attribute_info_t {
		GLuint index = 0;
		GLint size = 0;
		GLenum type = GL_FLOAT;
		GLboolean normalized = GL_FALSE;
		GLsizei stride = 0;
		const void* ptr = 0;
	};

	class CVertexAttribute {
	public:
		CVertexAttribute();
		~CVertexAttribute();

		bool Create();
		// You have to bind a vertex buffer first to use this
		void SetAttributeData(const attribute_info_t* stride_data, int num_attributes);
		void Bind();

	private:
		GLuint m_VertexArrayID;
		int m_Stride;
	};

}
#endif