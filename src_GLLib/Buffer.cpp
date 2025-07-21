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

	bool CreateVertexAttributes(int num_attributes, const attribute_info_t* attribute_data, const buffer_info_t* buffer_data, GLuint* vertex_attribute_id) {
		assert(nullptr != attribute_data);
		assert(nullptr != buffer_data);
		assert(nullptr != vertex_attribute_id);

		GLuint va = -1;
		glCreateVertexArrays(1, &va);
		if (-1 == va) return false;
		for (int i = 0; i < num_attributes; i++) {
			// Put attributes in its place
			if (attribute_data[i].type == GL_INT || attribute_data[i].type == GL_UNSIGNED_INT)
				glVertexArrayAttribIFormat(va, attribute_data[i].index, attribute_data[i].size, attribute_data[i].type, attribute_data[i].ptr);
			else
				glVertexArrayAttribFormat(va, attribute_data[i].index, attribute_data[i].size, attribute_data[i].type, attribute_data[i].normalized, attribute_data[i].ptr);

			// And enable the attribute
			glEnableVertexArrayAttrib(va, attribute_data[i].index);
		}
		glVertexArrayVertexBuffers(va, 0, num_attributes, buffer_data->buffer, buffer_data->offset, buffer_data->stride);
		*vertex_attribute_id = va;
		GL_ERROR_RETURN();
		return true;
	}

	void BindConstantBuffer(const GLuint buffer, int index) {
		assert(buffer > 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
		GL_ERROR();
	}

}