#ifndef TEXTURE_INCLUDED
#define TEXTURE_INCLUDED

#include <gl/glew.h>

namespace Clb184 {
	bool LoadTextureFromFile(const char* name, GLuint* tex_unit);
}

#endif