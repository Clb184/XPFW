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

	// Relevant structs
	z_stream stream;
	FILE* src = 0;
	uint8_t* data = 0;
	uint8_t* data_out = 0;
	size_t size = 0;

	src = fopen(filename, "rb");
	if(0 == src) return -1; // Failed to open file
	
	fseek(src, 0, SEEK_END);
	size = ftell(src);
	rewind(src);
	sprintf(buf, "Entry file size: %lld", size);
	LOG_INFO(buf);
	
	data = calloc(size, 1);
	data_out = calloc(128 * 1024, 1);

	if(0 == data || 0 == data_out) return -2; // Failed to allocate memory
	

	// Begin deflate
	if(Z_OK == deflateInit(&stream, -1)) {
		LOG_INFO("Begin entry compression");
		stream.data_type = Z_BINARY;
		while(stream.total_in != size) {
			stream.avail_in = fread(data, size, 1, src);
			stream.next_in = data;
			while(0 < stream.avail_in) {
				stream.avail_out = 128 * 1024;
				stream.next_out = data_out;
				deflate(&stream, Z_NO_FLUSH);
				sprintf(buf, "Available out bytes after deflate: %lld", stream.avail_out);
				LOG_INFO(buf);

				sprintf(buf, "Available in bytes after deflate: %lld", stream.avail_in);
				LOG_INFO(buf);
			}
		}
		if(Z_OK == deflateEnd(&stream)) {
			sprintf(buf, "Entry added successfuly, compressed to %lld bytes", stream.total_out);
			LOG_INFO(buf);
		}
	} else {
		LOG_ERROR("Error on initializing deflate operation");
	}
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
