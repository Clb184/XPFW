#include "PackArchive/PackFile.h"
#include "zlib.h"
#include "Output.h"
#include <assert.h>
#include <string.h>

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
	const int BUFFER_OUT_SIZE = 1024 * 1024;

	src = fopen(filename, "rb");
	if(0 == src) return -1; // Failed to open file
	
	fseek(src, 0, SEEK_END);
	size = ftell(src);
	rewind(src);
	sprintf(buf, "Entry file size: %lld", size);
	LOG_INFO(buf);
	
	data = calloc(size, 1);
	data_out = calloc(BUFFER_OUT_SIZE, 1);

	if(0 == data || 0 == data_out) return -2; // Failed to allocate memory
	

	// Begin deflate
	int64_t remaining_size = size;
	uint64_t offset = 0;
	int flush = Z_NO_FLUSH;
	int result = 0;
	uint64_t compressed_size = 0;
	stream.zalloc = ZMemAlloc;
	stream.zfree = ZMemFree;
	if(Z_OK == deflateInit(&stream, -1)) {
		LOG_INFO("Begin entry compression");
		stream.data_type = Z_BINARY;
		while(stream.total_in != size) {
			stream.avail_in = fread(data, 1, size, src);
			stream.next_in = data;
			for(int i = 0; i < 32; i++){
				printf("%2x ", data[i]);
			}
			printf("\n");
			sprintf(buf, "Data read: %d", stream.avail_in);
			LOG_INFO(buf);

			while(1) {
				remaining_size -= BUFFER_OUT_SIZE;
				stream.avail_out = BUFFER_OUT_SIZE;
				stream.next_out = data_out;
				printf("Remaining size: %lld\n", remaining_size);
				if(remaining_size <= 0) {
					flush = Z_FINISH;
				}
				result = deflate(&stream, flush);
				if(Z_OK != result) {
					sprintf(buf, "Failed calling deflate() with ID: %d MSG: %s", result, stream.msg);
					LOG_ERROR(buf);
				}
				for(int i = 0; i < 32; i++){
					printf("%2x ", data_out[i]);
				}
				offset += BUFFER_OUT_SIZE;
				printf("\n");
				sprintf(buf, "Available in bytes after deflate: %d", stream.avail_in);
				LOG_INFO(buf);
				sprintf(buf, "Available out bytes after deflate: %d", stream.avail_out);
				LOG_INFO(buf);
				sprintf(buf, "Total bytes in: %d", stream.total_in);
				LOG_INFO(buf);
				sprintf(buf, "Total bytes out: %d", stream.total_out);
				LOG_INFO(buf);
				if(stream.avail_out != 0 && remaining_size <= 0) break;
			}
		}
		result = deflateEnd(&stream);
		if(Z_OK == result) {
			compressed_size = stream.total_out;
			sprintf(buf, "Entry added successfuly, compressed to %lld bytes", stream.total_out);
			LOG_INFO(buf);

			// Test
			flush = Z_NO_FLUSH;
			remaining_size = compressed_size;
			stream.zalloc = ZMemAlloc;
			stream.zfree = ZMemFree;
			stream.avail_in = 0;
			stream.next_in = 0;
			memset(data, 0, size);
			if(Z_OK == inflateInit(&stream)) {
				printf("Inflate test:\n");

				while(compressed_size != stream.total_in) {
					stream.avail_in = compressed_size;
					stream.next_in = data_out;
					while(0 < stream.avail_in) {
						stream.avail_out = size;
						stream.next_out = data;
						remaining_size -= size;
						if(remaining_size <= 0) flush = Z_FINISH;
						inflate(&stream, flush);
					}
				
				}

				for(int i = 0; i < 32; i++){
					printf("%2x ", data[i]);
				}
				printf("\n");
				printf("Decompressed total of %d bytes\n", stream.total_out);	
				
			}

		} else {
			sprintf(buf, "An error ocurred while finishing deflate: ID: %d MSG: %s",  result, stream.msg);
			LOG_ERROR(buf);
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
