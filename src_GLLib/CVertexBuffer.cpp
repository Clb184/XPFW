#include "OpenGL/CVertexBuffer.hpp"

namespace Clb184 {
	CVertexBuffer::CVertexBuffer() {
		m_BufferID = -1;
	}

	CVertexBuffer::~CVertexBuffer() {
		DestroyBuffer();
	}

	bool CVertexBuffer::Create(size_t buffer_size, void* initial_data, GLenum usage) {
		GLuint vbuffer = -1, varray = -1;

		glCreateBuffers(1, &vbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
		glBufferData(GL_ARRAY_BUFFER, buffer_size, initial_data, usage);
		m_BufferID = vbuffer;
		return true;
	}


	void CVertexBuffer::Bind() {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void* CVertexBuffer::Lock() {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	}

	void CVertexBuffer::Unlock(size_t size) {
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void CVertexBuffer::DestroyBuffer() {
		if (glIsBuffer(m_BufferID)) {
			glDeleteBuffers(1, &m_BufferID);
		}
	}
}