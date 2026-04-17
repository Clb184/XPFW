#include "PackArchive/PackFile.h"
#include "zlib.h"
#include "Output.h"
#include <assert.h>

voidpf ZMemAlloc(voidpf custom, uInt cnt, uInt size) {
	char buf[512] = "";
	sprintf(buf, "Allocating ZMem %d bytes", size * cnt);
	LOG_INFO(buf);

	void* data = calloc(cnt, size);
	return data;
}

void ZMemFree(voidpf custom, voidpf data) {
	LOG_INFO("Freeing ZMem");
	free(data);
}

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
	stream.zalloc = ZMemAlloc;
	stream.zfree = ZMemFree;
	if(Z_OK == deflateInit(&stream, -1)) {
		LOG_INFO("Begin entry compression");
		stream.data_type = Z_BINARY;
		while(stream.total_in != size) {
			stream.avail_in = fread(data, 1, size, src);
			stream.next_in = data;
			for(int i = 0; i < 32; i++){
				printf("%x ", data[i]);
			}
			printf("\n");
			sprintf(buf, "Data read: %d", stream.avail_in);
			LOG_INFO(buf);

			while(0 < stream.avail_in) {
				stream.avail_out = 128 * 1024;
				stream.next_out = data_out;
				if(Z_OK != deflate(&stream, Z_NO_FLUSH)) {
					LOG_ERROR("Failed calling deflate()");
				}

				sprintf(buf, "Available in bytes after deflate: %d", stream.avail_in);
				LOG_INFO(buf);
				sprintf(buf, "Available out bytes after deflate: %d", stream.avail_out);
				LOG_INFO(buf);
				sprintf(buf, "Total bytes in: %d", stream.total_in);
				LOG_INFO(buf);
				sprintf(buf, "Total bytes out: %d", stream.total_out);
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
