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

	void CVertexAttribute::SetAttributeData(int num_attributes, const attribute_info_t* attribute_data, const buffer_info_t* buffer_data) {
		GLuint varray = m_VertexArrayID;
 		for (int i = 0; i < num_attributes; i++) {
			// Put attributes in its place
			if (attribute_data[i].type == GL_INT || attribute_data[i].type == GL_UNSIGNED_INT)
				glVertexArrayAttribIFormat(varray, attribute_data[i].index, attribute_data[i].size, attribute_data[i].type, attribute_data[i].ptr);
			else
				glVertexArrayAttribFormat(varray, attribute_data[i].index, attribute_data[i].size, attribute_data[i].type, attribute_data[i].normalized, attribute_data[i].ptr);

			// And enable the attribute
			glEnableVertexArrayAttrib(varray, attribute_data[i].index);
		}
		glVertexArrayVertexBuffers(varray, 0, num_attributes, buffer_data->buffer, buffer_data->offset, buffer_data->stride);
	}

	void CVertexAttribute::Bind() {
		glBindVertexArray(m_VertexArrayID);
	}
}