#include "OpenGL/CVertexBuffer.hpp"

namespace Clb184 {
	CVertexBuffer::CVertexBuffer() {
		m_BufferID = -1;
	}

	CVertexBuffer::~CVertexBuffer() {
		DestroyBuffer();
	}

	bool CVertexBuffer::Create(size_t buffer_size, void* initial_data, GLenum usage) {
		GLuint vbuffer = -1;

		glCreateBuffers(1, &vbuffer);
		glNamedBufferData(vbuffer, buffer_size, initial_data, usage);
		m_BufferID = vbuffer;
		return true;
	}


	void CVertexBuffer::Bind() {
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void* CVertexBuffer::Lock() {
		return glMapNamedBuffer(m_BufferID, GL_WRITE_ONLY);
	}

	void CVertexBuffer::Unlock(size_t size) {
		glUnmapNamedBuffer(m_BufferID);
	}

	GLuint CVertexBuffer::GetBufferID() const {
		return m_BufferID;
	}

	void CVertexBuffer::DestroyBuffer() {
		if (glIsBuffer(m_BufferID)) {
			glDeleteBuffers(1, &m_BufferID);
		}
	}
}