#ifndef PRIMITIVES_INCLUDED
#define PRIMITIVES_INCLUDED

#include <stdint.h>

// Just a single point
struct TVertex2D {
	float x = 0.0f;
	float y = 0.0f;
};

// Single point, with texture coordinates and color
struct TLVertex2D {
	float x = 0.0f;
	float y = 0.0f;
	float u = 0.0f;
	float v = 0.0f;
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
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float u = 0.0f;
	float v = 0.0f;
	uint32_t color = 0xffffffff;
};

#endif