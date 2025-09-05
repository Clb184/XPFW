#ifndef BUFFERCREATE_INCLUDED
#define BUFFERCREATE_INCLUDED

#include <GL/glew.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	GLsizei size;
	void* data;
	GLenum type;
}buffer_descriptor_t;

typedef struct  {
	GLuint index;// = 0;
	GLint size; // = 0;
	GLenum type; // = GL_FLOAT;
	GLboolean normalized; // = GL_FALSE;
	GLuint ptr; // = 0;
}attribute_info_t;

typedef struct  {
	GLuint buffer;// = -1;
	GLsizei stride;// = -1;
} buffer_info_t;

bool CreateBuffer(const buffer_descriptor_t* descriptor, GLuint* buffer_id); // Creates a single buffer
bool CreateBuffers(const buffer_descriptor_t* descriptors, GLuint* buffer_ids, int cnt); // Creates multiple buffers (Must pass an existing array)
bool CreateVertexAttribute(const attribute_info_t* attribute_data, const buffer_info_t* buffer_data, GLuint* vertex_array); // Create a vertex attribute buffer
void BindConstantBuffer(const GLuint buffer, int index); // Bind a buffer to a constant buffer slot (Shader Storage Buffer Object in this case)

#ifdef __cplusplus
}
#endif

#endif