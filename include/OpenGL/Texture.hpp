#ifndef TEXTURE_INCLUDED
#define TEXTURE_INCLUDED

#include <gl/glew.h>

namespace Clb184 {
	bool LoadTextureFromFile(const char* name, GLuint* tex_unit, int* width, int* height); // Simply loads a texture from file, optionally get the size of it via pointers
}

#endif