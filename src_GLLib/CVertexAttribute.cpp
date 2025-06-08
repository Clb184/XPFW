#include "OpenGL/CVertexAttribute.hpp"

namespace Clb184 {

	CVertexAttribute::CVertexAttribute() {
		m_VertexArrayID = -1;
	}

	CVertexAttribute::~CVertexAttribute() {
		if (glIsVertexArray(m_VertexArrayID)) {
			glDeleteVertexArrays(1, &m_VertexArrayID);
		}
	}

	bool CVertexAttribute::Create() {
		GLuint varray = -1;
		glCreateVertexArrays(1, &varray);
		glBindVertexArray(varray);
		m_VertexArrayID = varray;
		return true;
	}

	void CVertexAttribute::SetAttributeData(const attribute_info_t* stride_data, int num_attributes) {
		glBindVertexArray(m_VertexArrayID);
		for (int i = 0; i < num_attributes; i++) {
			// Put attributes in its place
			if (stride_data[i].type == GL_INT || stride_data[i].type == GL_UNSIGNED_INT) glVertexAttribIPointer(stride_data[i].index, stride_data[i].size, GL_INT, stride_data[i].stride, stride_data[i].ptr);
			else glVertexAttribPointer(stride_data[i].index, stride_data[i].size, stride_data[i].type, stride_data[i].normalized, stride_data[i].stride, stride_data[i].ptr);

			// And enable the attribute
			glEnableVertexAttribArray(stride_data[i].index);
		}
	}

	void CVertexAttribute::Bind() {
		glBindVertexArray(m_VertexArrayID);
	}
}