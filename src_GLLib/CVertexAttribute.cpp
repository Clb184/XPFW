#include "OpenGL/CVertexAttribute.hpp"

namespace Clb184 {

	CVertexAttribute::CVertexAttribute() {
		m_VertexArrayID = -1;
		m_Stride = 0;
	}

	CVertexAttribute::~CVertexAttribute() {
		if (glIsVertexArray(m_VertexArrayID)) {
			glDeleteVertexArrays(1, &m_VertexArrayID);
		}
	}

	bool CVertexAttribute::Create() {
		GLuint varray = -1;
		glCreateVertexArrays(1, &varray);
		m_VertexArrayID = varray;
		return true;
	}

	void CVertexAttribute::SetAttributeData(int num_attributes, const attribute_info_t* stride_data, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides) {
		GLuint varray = m_VertexArrayID;
 		for (int i = 0; i < num_attributes; i++) {
			// Put attributes in its place
			if (stride_data[i].type == GL_INT || stride_data[i].type == GL_UNSIGNED_INT)
				glVertexArrayAttribIFormat(varray, stride_data[i].index, stride_data[i].size, stride_data[i].type, stride_data[i].ptr);
			else
				glVertexArrayAttribFormat(varray, stride_data[i].index, stride_data[i].size, stride_data[i].type, stride_data[i].normalized, stride_data[i].ptr);

			// And enable the attribute
			glEnableVertexArrayAttrib(varray, stride_data[i].index);
		}
		glVertexArrayVertexBuffers(varray, 0, num_attributes, buffers,offsets, strides);
	}

	void CVertexAttribute::Bind() {
		glBindVertexArray(m_VertexArrayID);
	}
}