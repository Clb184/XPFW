#ifndef VERTEXBUFFER_INCLUDED
#define VERTEXBUFFER_INCLUDED

#include <GL/glew.h>
#include "IBuffer.hpp"
#include "CVertexAttribute.hpp"

namespace Clb184 {

	class CVertexBuffer : public IBuffer {
	public:
		CVertexBuffer();
		~CVertexBuffer();

		bool Create(size_t buffer_size, void* initial_data, GLenum usage);
		void Bind();
		void* Lock();
		void Unlock(size_t size);

		GLuint GetBufferID() const;

	private:
		void DestroyBuffer();

	private:
		GLuint m_BufferID;
	};

}
#endif