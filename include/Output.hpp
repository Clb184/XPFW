#ifndef OUTPUT_INCLUDED 
#define OUTPUT_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#define LOG_INFO(x) fprintf(stdout, "[INFO] %s : %d - %s | %s\n", __FILE__, __LINE__, __func__, x)
#define LOG_ERROR(x) fprintf(stderr, "[ERROR] %s : %d - %s | %s\n", __FILE__, __LINE__, __func__, x)

#endif