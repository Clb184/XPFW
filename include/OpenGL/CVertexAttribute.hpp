#ifndef VERTEXATTRIBUTE_INCLUDED
#define VERTEXATTRIBUTE_INCLUDED

#include <GL/glew.h>
#include "Buffer.hpp"

namespace Clb184 {

	class CVertexAttribute {
	public:
		CVertexAttribute();
		~CVertexAttribute();

		bool Create();
		// You have to bind a vertex buffer first to use this
		void SetAttributeData(int num_attributes, const attribute_info_t* stride_data, const buffer_info_t* buffer_data);
		void Bind();

	private:
		GLuint m_VertexArrayID;
		int m_Stride;
	};

}
#endif