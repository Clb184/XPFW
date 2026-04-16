#include "PackArchive/PackFile.h"
#include "zlib.h"
#include "Output.h"
#include <assert.h>


int PackFileOpen(pack_file_t* pack_file, const char* filename) {
	char buf[1024];
	sprintf(buf, "Opening packed file \"%s\"", filename);
	LOG_INFO(buf);
	FILE* file = 0;
	return 0;
}

int PackFileCreate(pack_file_t* pack_file){
	LOG_INFO("Creating packed file");
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
	char buf[1024];
	sprintf(buf, "Adding entry with data from file \"%s\" to packed file", filename);
	LOG_INFO(buf);
	return 0;
}

int PackFileAddEntryFromMemory(pack_file_t* pack_file, char* data, const char* filename){
	char buf[1024];
	sprintf(buf, "Adding entry with data from memory with name \"%s\"", filename);
	LOG_INFO(buf);

	return 0;
}

int PackFileWrite(pack_file_t* pack_file, const char* filename){
	char buf[1024];
	sprintf(buf, "Writting packed file \"%s\"", filename);
	LOG_INFO(buf);

	return 0;
}
