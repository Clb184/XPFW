#ifndef BUFFERCREATE_INCLUDED
#define BUFFERCREATE_INCLUDED

#include <GL/glew.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	GLsizei size; // Size of buffer in bytes
	void* data; // Initial data (Can be nullptr for uninitialized)
	GLenum type; // Type of buffer ( Some are ARRAY_BUFFER, DRAW_INDIRECT_BUFFER, SHADER_STORAGE_BUFFER, UNIFORM_BUFFER, etc )
} buffer_descriptor_t;

typedef struct  {
	GLuint index;// = 0; // Index of attribute
	GLint size; // = 0; // How many of these (Will be multiplied by corresponding type size)
	GLenum type; // = GL_FLOAT; // The type, like GL_INT, GL_FLOAT, GL_DOUBLE, etc
	GLboolean normalized; // = GL_FALSE; // Clamp values between a range ( Floats between 0.0 to 1.0)
	GLuint ptr; // = 0; // Offset to data
} attribute_info_t;

typedef struct  {
	GLuint buffer;// = -1 // Named Buffer to use
	GLsizei stride;// = -1; // Stride between properties
} buffer_info_t;

bool CreateBuffer(const buffer_descriptor_t descriptor, GLuint* buffer_id); // Creates a single buffer
bool CreateBuffers(const buffer_descriptor_t* descriptors, GLuint* buffer_ids, int cnt); // Creates multiple buffers (Must pass an existing array)
bool CreateVertexAttribute(const attribute_info_t attribute_data, const buffer_info_t buffer_data, GLuint* vertex_array); // Create a vertex attribute buffer
void BindConstantBuffer(const GLuint buffer, int index); // Bind a buffer to a constant buffer slot (Shader Storage Buffer Object in this case)

#ifdef __cplusplus
}
#endif

#endif
