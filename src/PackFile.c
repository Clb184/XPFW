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
	pack_file->entry_count = 0;
	pack_file->entry_max = 0;
	pack_file->header.magic[0] = 'C';
	pack_file->header.magic[1] = 'A';
	pack_file->header.magic[2] = 'F';
	pack_file->header.magic[3] = '0';
	pack_file->header.flags = 0;
	pack_file->header.checksum = 0;
	pack_file->header.entry_count = 0;
	pack_file->entries = 0;
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
	fread(data, 1, size, src);
	fclose(src);

	if(0 == data) return -2; // Failed to allocate memory
	

	// Begin deflate
	uint64_t buffer_out_size = 0;
	int result = 0;
	uint64_t compressed_size = 0;
	stream.zalloc = ZMemAlloc;
	stream.zfree = ZMemFree;
	if(Z_OK == deflateInit(&stream, -1)) {
		LOG_INFO("Begin entry compression");
		stream.data_type = Z_BINARY;
		stream.avail_in = size;
		stream.next_in = data;
		for(int i = 0; i < 32; i++){
			printf("%2x ", data[i]);
		}
		printf("\n");
		sprintf(buf, "Stream available bytes: %d", stream.avail_in);
		LOG_INFO(buf);

		// Prepare to compress all
		buffer_out_size = deflateBound(&stream, size);
		sprintf(buf, "Expected deflate size: %lld", buffer_out_size);
		
		// Output buffer
		data_out = calloc(buffer_out_size, 1);

		if(0 == data_out) {
			deflateEnd(&stream);
			return -3;
		}

		stream.avail_out = buffer_out_size;
		stream.next_out = data_out;
		result = deflate(&stream, Z_FINISH);

		sprintf(buf, "Deflate returned: %d", result);
		LOG_INFO(buf);
		sprintf(buf, "Stream total out: %d, Buffer out: %lld", stream.total_out, buffer_out_size);
		LOG_INFO(buf);

		if(Z_STREAM_END == result) {
			LOG_INFO("Deflate successful");
			compressed_size = stream.total_out;
		}

		result = deflateEnd(&stream);

		// End compression
		if(Z_OK == result) {
			sprintf(buf, "Entry added successfuly, compressed to %lld bytes", compressed_size);
			LOG_INFO(buf);

			// Decompress data to check integrity
			stream.zalloc = ZMemAlloc;
			stream.zfree = ZMemFree;
			stream.avail_in = 0;
			stream.next_in = 0;
			memset(data, 0, size);
			if(Z_OK == inflateInit(&stream)) {
				printf("Inflate test:\n");
				
				// Prepare full inflate
				stream.avail_in = buffer_out_size;
				stream.next_in = data_out;
				stream.avail_out = size;
				stream.next_out = data;
				result = inflate(&stream, Z_FINISH);

				if(Z_STREAM_END == result) {
					LOG_INFO("Inflate test passed");
				}
				else {
					sprintf(buf, "Inflate test failed: ID %d", result);
					LOG_ERROR(buf);
				}

				result = inflateEnd(&stream);

				for(int i = 0; i < 32; i++){
					printf("%2x ", data[i]);
				}
				printf("\n");

				printf("Decompressed total of %d bytes\n", stream.total_out);	
				if(Z_OK == result && stream.total_out == size) {
					LOG_INFO("Decompression success");
					pack_file_entry_t entry;
					entry.name = filename;
					entry.output_size = size;
					entry.this_size = buffer_out_size;
					entry.checksum = 0;
					entry.data = data_out;
					if(0 == PackFileDoAddEntry(pack_file, &entry)) {
						LOG_INFO("Success on adding entry");
					} else {
						LOG_ERROR("Failed adding entry");
					}
				}
				else {
				sprintf(buf, "An error ocurred while finishing inflate: ID: %d MSG: %s",  result, stream.msg);
			LOG_ERROR(buf);

				}
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
	pack_file->header.entry_count = pack_file->entry_count;
	FILE* output = 0;

	output = fopen(filename, "wb");
	if(0 == output) {
		LOG_ERROR("Error writting pack file");
		return -1;
	}

	fwrite(&pack_file->header, sizeof(pack_file_header_t), 1, output);
	fwrite(pack_file->entries, sizeof(pack_file_entry_t), pack_file->entry_count, output);
	fclose(output);


	return 0;
}

int PackFileLoadEntry(pack_file_t* pack_file, const char* entry_name) {
	char buf[1024];
	sprintf(buf, "Loading packed file entry \"%s\"", entry_name);
	LOG_INFO(buf);

	z_stream stream;
	int result = 0;
	pack_file_entry_t* entry = 0;

	if(0 == entry) {
		sprintf(buf, "Entry \"%s\" not found", entry_name);
		LOG_ERROR(buf);
		return -1;
	}

	// Decompress data
	stream.zalloc = ZMemAlloc;
	stream.zfree = ZMemFree;
	stream.avail_in = 0;
	stream.next_in = 0;
	if(Z_OK == inflateInit(&stream)) {
		printf("Inflate data:\n");
		
		uint8_t* data_out = calloc(entry->output_size, 1);

		// Prepare full inflate
		stream.avail_in = entry->this_size;
		stream.next_in = entry->data;
		stream.avail_out = entry->output_size;
		stream.next_out = data_out;
		result = inflate(&stream, Z_FINISH);

		if(Z_STREAM_END == result) {
			LOG_INFO("Inflate passed");
		}
		else {
			sprintf(buf, "Inflate failed: ID %d", result);
			LOG_ERROR(buf);
		}

		result = inflateEnd(&stream);

		for(int i = 0; i < 32; i++){
			printf("%2x ", entry->data[i]);
		}
		printf("\n");

		printf("Decompressed total of %d bytes\n", stream.total_out);	
		if(Z_OK == result && stream.total_out == entry->output_size) {
			LOG_INFO("Decompression success");
		}
		else {
			sprintf(buf, "An error ocurred while finishing inflate: ID: %d MSG: %s",  result, stream.msg);
			LOG_ERROR(buf);

		}
	}
	return 0;
}

int PackFileFindEntry(pack_file_t* pack_file, const char* entry_name, pack_file_entry_t** out) {
	char buf[1024];
	sprintf(buf, "Looking for file entry \"%s\"", entry_name);
	LOG_INFO(buf);

	assert(0 != out);
	if(0 == out) {
		return -1;
	}
	
	// Iterate and find same name
	uint64_t entries = pack_file->header.entry_count;
	for (int64_t i = 0; i < entries; i++) {
		if(0 == (strcmp(entry_name, pack_file->entries[i].name))){
			*out = pack_file->entries + i;
			return 0;
		}
	}

	return -2;
}

int PackFileDoAddEntry(pack_file_t* pack_file, pack_file_entry_t* entry) {
	LOG_INFO("Adding entry to pack file");

	if (pack_file->entry_max > 0 && pack_file->entry_count < pack_file->entry_max) {
		pack_file->entries[pack_file->entry_count] = *entry;
		pack_file->entry_count++;
	} else if (pack_file->entry_max <= 0) {
		pack_file->entries = calloc(4, sizeof(pack_file_entry_t));
		pack_file->entry_max = 4;
		pack_file->entry_count = 1;
		pack_file->entries[0] = *entry;
	} else if (pack_file->entry_count >= pack_file->entry_max) {
		pack_file->entry_max *= 2;
		pack_file->entries = realloc(pack_file->entries, sizeof(pack_file->entry_max));
		pack_file->entries[pack_file->entry_count] = *entry;
		pack_file->entry_count++;
	}

	return 0;
}
