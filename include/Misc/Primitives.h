#ifndef PRIMITIVES_INCLUDED
#define PRIMITIVES_INCLUDED

#include <stdint.h>
#include <assert.h>
#include "Output.h"
#include "../OpenGL/Buffer.h"

/********************************************************************
* Note: These are just my default primitives to just drop and use
* Mostly inspired by the T&L stuff since is just all I need
********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

	// Just a single point
	typedef struct  {
		float x;// = 0.0f;
		float y;// = 0.0f;
	}TVertex2D;

	const int T2DAttributeCount = 3; // Pos, UV, Color
	static attribute_info_t g_T2DAttributes[] = {
		{0, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 0)},
	};


	// Single point, with texture coordinates and color
	typedef struct  {
		// Vertex position
		float x; // = 0.0f;
		float y; // = 0.0f;

		// Texture coordinates
		float u; // = 0.0f;
		float v; // = 0.0f;

		// Color (ARGB)
		uint32_t color; // = 0xffffffff;
	} TLVertex2D;

	inline void CreateTL2DVertexBuffer(size_t num_vertex, TLVertex2D* pdata, GLenum usage, GLuint* vbuffer, GLuint* vattribute) {
		assert(0 != vbuffer);
		assert(0 != vattribute);

		buffer_descriptor_t vbd = { num_vertex * sizeof(TLVertex2D), pdata, usage };
		GLuint vb = -1;
		CreateBuffer(&vbd, &vb);

		GLuint va = -1;
		glCreateVertexArrays(1, &va);
		const int TL2DAttributeCount = 3; // Pos, UV, Color
		attribute_info_t TL2DAttributes[] = {
			{0, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 0)},
			{1, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 2)},
			{2, 4, GL_UNSIGNED_BYTE, GL_TRUE, (sizeof(float) * 4)}
		};

		buffer_info_t buffinfo = { vb, sizeof(TLVertex2D)};

		CreateVertexAttribute(&TL2DAttributes[0], &buffinfo, &va);
		CreateVertexAttribute(&TL2DAttributes[1], &buffinfo, &va);
		CreateVertexAttribute(&TL2DAttributes[2], &buffinfo, &va);

		*vbuffer = vb;
		*vattribute = va;
	}

	// 3D point
	typedef struct {
		float x; // = 0.0f;
		float y; // = 0.0f;
		float z; // = 0.0f;
	} TVertex3D;

	const int T3DAttributeCount = 4; // Pos, UV, Color
	static attribute_info_t g_T3DAttributes[] = {
		{0, 3, GL_FLOAT, GL_FALSE, (sizeof(float) * 0)},
	};

	// 3D point with texture coordinates and color
	typedef struct  {
		// Vertex position
		float x; // = 0.0f;
		float y; // = 0.0f;
		float z; // = 0.0f;

		// Diffuse color
		uint32_t color; // = 0xffffffff;

		// Texture coordinates
		float u; // = 0.0f;
		float v; // = 0.0f;

		// Normals
		float i; // = 0.0f;
		float j; // = 0.0f;
		float k; // = 0.0f;
	} TLVertex3D;


	inline void CreateTL3DVertexBuffer(size_t num_vertex, TLVertex3D* pdata, GLenum usage, GLuint* vbuffer, GLuint* vattribute) {
		assert(0 != vbuffer);
		assert(0 != vattribute);

		buffer_descriptor_t vbd = { num_vertex * sizeof(TLVertex3D), pdata, usage};
		GLuint vb = -1;
		CreateBuffer(&vbd, &vb);

		GLuint va = -1;
		glCreateVertexArrays(1, &va);
		const int TL3DAttributeCount = 4; // Pos, UV, Color
		attribute_info_t TL3DAttributes[] = {
		{0, 3, GL_FLOAT, GL_FALSE,(sizeof(float) * 0)},
		{1, 4, GL_UNSIGNED_BYTE, GL_TRUE, (sizeof(float) * 3)},
		{2, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 4)},
		{3, 3, GL_FLOAT, GL_FALSE, (sizeof(float) * 6)},
		};

		buffer_info_t buffinfo = { vb, sizeof(TLVertex3D) };

		CreateVertexAttribute(&TL3DAttributes[0], &buffinfo, &va);
		CreateVertexAttribute(&TL3DAttributes[1], &buffinfo, &va);
		CreateVertexAttribute(&TL3DAttributes[2], &buffinfo, &va);
		CreateVertexAttribute(&TL3DAttributes[3], &buffinfo, &va);

		*vbuffer = vb;
		*vattribute = va;
	}

#ifdef __cplusplus
}
#endif

#endif