#include "OpenGL/Sampler.h"
#include <assert.h>
#include "Output.h"

bool CreateSampler(GLuint* unit) {
	LOG_INFO("Creating Sampler");
	assert(0 != unit);
	GLERR;
	*unit = 0xffffffff;
	glCreateSamplers(1, unit);
	GL_ERROR_RETURN();
	return true;
}

void SetSamplerWrapMode(GLuint unit, GLuint wrapud, GLuint wraplr) {
	glSamplerParameteri(unit, GL_TEXTURE_WRAP_S, wraplr);
	glSamplerParameteri(unit, GL_TEXTURE_WRAP_T, wrapud);
}

void SetSamplerTextureMode(GLuint unit, GLuint mode) {
	glSamplerParameteri(unit, GL_TEXTURE_MIN_FILTER, mode);
	glSamplerParameteri(unit, GL_TEXTURE_MAG_FILTER, mode);
}