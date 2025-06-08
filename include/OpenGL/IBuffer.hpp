#ifndef IBUFFER_INCLUDED
#define IBUFFER_INCLUDED

namespace Clb184 {
	class IBuffer {
	public:
		virtual bool Create(size_t buffer_size, void* initial_data, GLenum usage) = 0;
		virtual void Bind() = 0;
		virtual void* Lock() = 0;
		virtual void Unlock(size_t size) = 0;
	protected:
		virtual void DestroyBuffer() = 0;
	};

}
#endif