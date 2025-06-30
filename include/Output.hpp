#ifndef OUTPUT_INCLUDED 
#define OUTPUT_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#define LOG_INFO(x) fprintf(stdout, "[INFO] %s : %d - %s | %s\n", __FILE__, __LINE__, __func__, x)
#define LOG_ERROR(x) fprintf(stderr, "[ERROR] %s : %d - %s | %s\n", __FILE__, __LINE__, __func__, x)


#define GL_ERROR() while (GLenum e = glGetError()) {\
	fprintf(stderr, "[GL ERROR] %s : %d > %s() failed with code %d", __FILE__, __LINE__, __func__, e);\
}

#define GL_ERROR_RETURN() while (GLenum e = glGetError()) {\
	fprintf(stderr, "[GL ERROR] %s : %d > %s() failed with code %d", __FILE__, __LINE__, __func__, e);\
	return false;\
}

#endif