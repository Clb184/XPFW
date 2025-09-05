#include "IO.h"
#include <assert.h>

bool LoadDataFromFile(const char* file_name, void** src, size_t* size) {
	assert(src != 0);
	size_t sz;
	FILE* fp = 0;

	if (fp = fopen(file_name, "rb")) {
		fseek(fp, 0, SEEK_END);
		sz = ftell(fp);
		rewind(fp);

		void* data = malloc(sz);
		if (data) {
			fread(data, sz, 1, fp);
			if (size) {
				*size = sz;
			}
			fclose(fp);
		}
		*src = data;
		return true;
	}

	return false;
}

bool LoadTextFromFile(const char* file_name, char** src, size_t* size) {
	assert(src != 0);
	size_t sz;
	FILE* fp = 0;

	if (fp = fopen(file_name, "r")) {
		fseek(fp, 0, SEEK_END);
		sz = ftell(fp);
		rewind(fp);

		void* data = calloc(sz, 1);
		if (data) {
			fread(data, sz, 1, fp);
			if (size) {
				*size = sz;
			}
			*src = (char*)data;
			fclose(fp);
		}
		return true;
	}

	return false;
}