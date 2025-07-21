#ifndef	SAMPLER_INCLUDED
#define SAMPLER_INCLUDED

#include <GL/glew.h>

namespace Clb184 {
	/*
	I'm mostly using 2D textures, so there's not much to be done here
	*/

	bool CreateSampler(GLuint* unit);
	void SetSamplerWrapMode(GLuint unit, GLuint wrapud, GLuint wraplr);
	void SetSamplerTextureMode(GLuint unit, GLuint mode); // Nearest or Linear
}

#endif