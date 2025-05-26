#ifndef PRIMITIVES_INCLUDED
#define PRIMITIVES_INCLUDED

#include <stdint.h>

/********************************************************************
* Note: These are just my default primitives to just drop and use
* Mostly inspired by the T&L stuff since is just all I need
********************************************************************/

// Just a single point
struct TVertex2D {
	float x = 0.0f;
	float y = 0.0f;
};

// Single point, with texture coordinates and color
struct TLVertex2D {
	// Vertex position
	float x = 0.0f;
	float y = 0.0f;

	// Texture coordinates
	float u = 0.0f;
	float v = 0.0f;

	// Color
	uint32_t color = 0xffffffff;
};

// 3D point
struct TVertex3D {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

// 3D point with texture coordinates and color
struct TLVertex2D {
	// Vertex position
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	
	// Texture coordinates
	float u = 0.0f;
	float v = 0.0f;

	// Diffuse color
	uint32_t color = 0xffffffff;

	// Normals
	float i = 0.0f;
	float j = 0.0f;
	float k = 0.0f;
};

#endif