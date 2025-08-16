#ifndef PRIMITIVES_INCLUDED
#define PRIMITIVES_INCLUDED

#include <stdint.h>
#include <assert.h>
#include "Output.hpp"
#include "../OpenGL/Buffer.hpp"

/********************************************************************
* Note: These are just my default primitives to just drop and use
* Mostly inspired by the T&L stuff since is just all I need
********************************************************************/
namespace Clb184 {
	// Just a single point
	struct TVertex2D {
		float x = 0.0f;
		float y = 0.0f;
	};

	constexpr int T2DAttributeCount = 3; // Pos, UV, Color
	static attribute_info_t g_T2DAttributes[] = {
		{0, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 0)},
	};


	// Single point, with texture coordinates and color
	struct TLVertex2D {
		// Vertex position
		float x = 0.0f;
		float y = 0.0f;

		// Texture coordinates
		float u = 0.0f;
		float v = 0.0f;

		// Color (ARGB)
		uint32_t color = 0xffffffff;
	};

	inline void CreateTL2DVertexBuffer(size_t num_vertex, TLVertex2D* pdata, GLenum usage, GLuint* vbuffer, GLuint* vattribute) {
		assert(nullptr != vbuffer);
		assert(nullptr != vattribute);

		buffer_descriptor_t vbd = { num_vertex * sizeof(TLVertex2D), pdata, usage };
		GLuint vb = -1;
		CreateBuffer(&vbd, &vb);

		GLuint va = -1;
		glCreateVertexArrays(1, &va);
		constexpr int TL2DAttributeCount = 3; // Pos, UV, Color
		attribute_info_t TL2DAttributes[] = {
			{0, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 0)},
			{1, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 2)},
			{2, 4, GL_UNSIGNED_BYTE, GL_TRUE, (sizeof(float) * 4)}
		};

		buffer_info_t buffinfo = { vb, sizeof(Clb184::TLVertex2D)};

		CreateVertexAttribute(&TL2DAttributes[0], &buffinfo, &va);
		CreateVertexAttribute(&TL2DAttributes[1], &buffinfo, &va);
		CreateVertexAttribute(&TL2DAttributes[2], &buffinfo, &va);

		*vbuffer = vb;
		*vattribute = va;
	}

	// 3D point
	struct TVertex3D {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	constexpr int T3DAttributeCount = 4; // Pos, UV, Color
	static attribute_info_t g_T3DAttributes[] = {
		{0, 3, GL_FLOAT, GL_FALSE, (sizeof(float) * 0)},
	};

	// 3D point with texture coordinates and color
	struct TLVertex3D {
		// Vertex position
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		// Diffuse color
		uint32_t color = 0xffffffff;

		// Texture coordinates
		float u = 0.0f;
		float v = 0.0f;

		// Normals
		float i = 0.0f;
		float j = 0.0f;
		float k = 0.0f;
	};


	inline void CreateTL3DVertexBuffer(size_t num_vertex, TLVertex3D* pdata, GLenum usage, GLuint* vbuffer, GLuint* vattribute) {
		assert(nullptr != vbuffer);
		assert(nullptr != vattribute);

		buffer_descriptor_t vbd = { num_vertex * sizeof(TLVertex3D), pdata, usage};
		GLuint vb = -1;
		CreateBuffer(&vbd, &vb);

		GLuint va = -1;
		glCreateVertexArrays(1, &va);
		constexpr int TL3DAttributeCount = 4; // Pos, UV, Color
		attribute_info_t TL3DAttributes[] = {
		{0, 3, GL_FLOAT, GL_FALSE,(sizeof(float) * 0)},
		{1, 4, GL_UNSIGNED_BYTE, GL_TRUE, (sizeof(float) * 3)},
		{2, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 4)},
		{3, 3, GL_FLOAT, GL_FALSE, (sizeof(float) * 6)},
		};

		buffer_info_t buffinfo = { vb, sizeof(Clb184::TLVertex3D) };

		CreateVertexAttribute(&TL3DAttributes[0], &buffinfo, &va);
		CreateVertexAttribute(&TL3DAttributes[1], &buffinfo, &va);
		CreateVertexAttribute(&TL3DAttributes[2], &buffinfo, &va);
		CreateVertexAttribute(&TL3DAttributes[3], &buffinfo, &va);

		*vbuffer = vb;
		*vattribute = va;
	}
}
#endif