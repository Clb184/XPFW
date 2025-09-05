#ifndef IO_INCLUDED
#define IO_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
bool LoadDataFromFile(const char* file_name, void** src, size_t* size); // Load data from file, free memory with free()
bool LoadTextFromFile(const char* file_name, char** src, size_t* size); // Load data as text from file, free memory with free()

#ifdef __cplusplus
}
#endif

#endif