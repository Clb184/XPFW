#include "OpenGL/Buffer.hpp"
#include "Output.hpp"
#include <assert.h>

namespace Clb184 {

	// Creates a buffer
	bool CreateBuffer(const buffer_descriptor_t* descriptor, GLuint* buffer_id) {
		LOG_INFO("Creating Buffer");
		assert(nullptr != descriptor);
		assert(nullptr != buffer_id);
		GLuint buf = -1;
		glCreateBuffers(1, &buf);
		if (-1 == buf) return false;
		glNamedBufferData(buf, descriptor->size, descriptor->data, descriptor->type);
		*buffer_id = buf;

		GL_ERROR_RETURN();

		return true;
	}

	// Same as above, but creates multiple buffers
	bool CreateBuffers(const buffer_descriptor_t* descriptors, GLuint* buffer_ids, int cnt) {
		LOG_INFO("Creating Buffers");
		assert(nullptr != descriptors);
		assert(nullptr != buffer_ids);

		glCreateBuffers(cnt, buffer_ids);
		for (int i = 0; i < cnt; i++) {
			glNamedBufferData(buffer_ids[i], descriptors[i].size, descriptors[i].data, descriptors[i].type);
		}

		GL_ERROR_RETURN();

		return true;
	}

	bool CreateVertexAttribute(const attribute_info_t* attribute_data, const buffer_info_t* buffer_data, GLuint* vertex_array) {
		assert(nullptr != attribute_data);
		assert(nullptr != buffer_data);
		assert(nullptr != vertex_array);
		assert(glIsVertexArray(*vertex_array));
		GLuint va = *vertex_array;
		if (-1 == va) return false;
		
		// Direct State Access setup of Vertex Array Attribute

		// Put attributes in its place
		if (attribute_data->type == GL_INT || attribute_data->type == GL_UNSIGNED_INT)
			glVertexArrayAttribIFormat(va, attribute_data->index, attribute_data->size, attribute_data->type, attribute_data->ptr);
		else
			glVertexArrayAttribFormat(va, attribute_data->index, attribute_data->size, attribute_data->type, attribute_data->normalized, attribute_data->ptr);

		// And enable the attribute
		glEnableVertexArrayAttrib(va, attribute_data->index);
		glVertexArrayVertexBuffer(va, attribute_data->index, buffer_data->buffer, buffer_data->offset, buffer_data->stride);

		// Is the same as the one above, but offsets are in different parts, however result is the same
		// I'm using only DSA (Direct State Access, so I'm not going to do the following that I'll keep commented out)
		/*
		glBindVertexArray(va);
		glBindBuffer(GL_ARRAY_BUFFER, buffer_data->buffer);
		if (attribute_data->type == GL_INT || attribute_data->type == GL_UNSIGNED_INT)
			glVertexAttribIPointer(attribute_data->index, attribute_data->size, attribute_data->type, buffer_data->stride, (const void*)attribute_data->ptr);
		else
			glVertexAttribPointer(attribute_data->index, attribute_data->size, attribute_data->type, attribute_data->normalized, buffer_data->stride, (const void*)attribute_data->ptr);

		glEnableVertexAttribArray(attribute_data->index);*/

		GL_ERROR_RETURN();
		return true;
	}

	void BindConstantBuffer(const GLuint buffer, int index) {
		assert(buffer > 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
		GL_ERROR();
	}

}