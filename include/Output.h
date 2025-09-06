#ifndef OUTPUT_INCLUDED 
#define OUTPUT_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#ifdef _DEBUG

#define LOG_INFO(x) fprintf(stdout, "[INFO] %s : %d - %s() | %s\n", __FILE__, __LINE__, __func__, x)
#define LOG_ERROR(x) fprintf(stderr, "[ERROR] %s : %d - %s() | %s\n", __FILE__, __LINE__, __func__, x)


#define GL_ERROR() while (e = glGetError()) {\
	fprintf(stderr, "[GL ERROR] %s : %d > %s() failed with code %d\n", __FILE__, __LINE__, __func__, e);\
}

#define GL_ERROR_RETURN() while (e = glGetError()) {\
	fprintf(stderr, "[GL ERROR] %s : %d > %s() failed with code %d\n", __FILE__, __LINE__, __func__, e);\
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