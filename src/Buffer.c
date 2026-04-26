#include "OpenGL/Buffer.h"
#include "Output.h"
#include <assert.h>

// Creates a buffer
bool CreateBuffer(const buffer_descriptor_t descriptor, GLuint* buffer_id) {
	LOG_INFO("Creating Buffer");
	char buff[512];
	//assert(0 != descriptor);
	assert(0 != buffer_id);
	GLERR;

	GLuint buf = -1;
	glCreateBuffers(1, &buf);
	if (-1 == buf) return false;
	glNamedBufferData(buf, descriptor.size, descriptor.data, descriptor.type);
	*buffer_id = buf;

	GL_ERROR_RETURN();
	sprintf(buff, "Created buffer id: %lld size: %lld data from: %p", buf, descriptor.size, descriptor.data);
	LOG_INFO(buff);
	return true;
}

// Same as above, but creates multiple buffers
bool CreateBuffers(const buffer_descriptor_t* descriptors, GLuint* buffer_ids, int cnt) {
	LOG_INFO("Creating Buffers");
	assert(0 != descriptors);
	assert(0 != buffer_ids);
	GLERR;

	glCreateBuffers(cnt, buffer_ids);
	for (int i = 0; i < cnt; i++) {
		glNamedBufferData(buffer_ids[i], descriptors[i].size, descriptors[i].data, descriptors[i].type);
	}

	GL_ERROR_RETURN();

	return true;
}

bool CreateVertexAttribute(const attribute_info_t attribute_data, const buffer_info_t buffer_data, GLuint* vertex_array) {
	//assert(0 != attribute_data);
	//assert(0 != buffer_data);
	assert(0 != vertex_array);
	assert(glIsVertexArray(*vertex_array));
	GLERR;

	GLuint va = *vertex_array;
	if (-1 == va) return false;
	
	// Direct State Access setup of Vertex Array Attribute

	// Put attributes in its place
	if (attribute_data.type == GL_INT || attribute_data.type == GL_UNSIGNED_INT)
		glVertexArrayAttribIFormat(va, attribute_data.index, attribute_data.size, attribute_data.type, 0);
	else
		glVertexArrayAttribFormat(va, attribute_data.index, attribute_data.size, attribute_data.type, attribute_data.normalized, 0);

	// And enable the attribute
	glEnableVertexArrayAttrib(va, attribute_data.index);
	glVertexArrayVertexBuffer(va, attribute_data.index, buffer_data.buffer, attribute_data.ptr, buffer_data.stride);

	GL_ERROR_RETURN();
	return true;
}

void BindConstantBuffer(const GLuint buffer, int index) {
	assert(buffer > 0);
	GLERR;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
	GL_ERROR();
}
