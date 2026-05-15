#ifndef OUTPUT_INCLUDED 
#define OUTPUT_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#ifdef _DEBUG

#define LOG_INFO(x) fprintf(stdout, "[INFO] %s : %d - %s() | %s\n", __FILE__, __LINE__, __func__, x)
#define LOG_ERROR(x) fprintf(stderr, "[ERROR] %s : %d - %s() | %s\n", __FILE__, __LINE__, __func__, x)

static const char* GetGLErrorValueString(int e) {

	switch(e) {
		case 0x0500: return "INVALID ENUM";
		case 0x0501: return "INVALID_VALUE";
		case 0x0502: return "INVALID OPERATION";
		case 0x0503: return "STACK OVERFLOW";
		case 0x0504: return "STACK UNDERFLOW";
		case 0x0505: return "OUT OF MEMORY";
		case 0x0506: return "INVALID FRAMEBUFFER OPERATION";
		case 0x0507: return "CONTEXT LOST";
		default: return "Unknown Error";
	}
}

#define GL_ERROR() while (e = glGetError()) {\
	fprintf(stderr, "[GL ERROR] (%s) @ %s : %d > %s()\n", GetGLErrorValueString(e), __FILE__, __LINE__, __func__);\
}

#define GL_ERROR_RETURN() while (e = glGetError()) {\
	fprintf(stderr, "[GL ERROR] (%s) @ %s : %d > %s()\n", GetGLErrorValueString(e), __FILE__, __LINE__, __func__);\
	return false;\
}

#define GLERR GLenum e

#else

#define LOG_INFO(x)
#define LOG_ERROR(x)


#define GL_ERROR()

#define GL_ERROR_RETURN() 

#define GLERR


#endif

#endif
