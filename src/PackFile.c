#include "PackArchive/PackFile.h"
#include "zlib.h"
#include <assert.h>

int PackFileOpen(pack_file_t* pack_file, const char* filename) {
	FILE* file = 0;
	return 0;
}

int PackFileCreate(pack_file_t* pack_file){
	assert(0 != pack_file);
	pack_file->file = 0;
	pack_file->file_size = 0;
	pack_file->header.magic[0] = 'C';
	pack_file->header.magic[1] = 'A';
	pack_file->header.magic[2] = 'F';
	pack_file->header.magic[3] = '0';
	return 0;
}

int PackFileAddEntryFromFile(pack_file_t* pack_file, const char* filename){
	return 0;
}

int PackFileAddEntryFromMemory(pack_file_t* pack_file, char* data, const char* filename){

	return 0;
}

int PackFileWrite(pack_file_t* pack_file, const char* filename){

	return 0;
}
