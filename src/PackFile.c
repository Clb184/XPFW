#include "PackArchive/PackFile.h"
#include "zlib.h"
#include "Output.h"
#include <assert.h>
#include <string.h>

voidpf ZMemAlloc(voidpf custom, uInt cnt, uInt size) {
#ifdef ZMEM_LOG
	char buf[512] = "";
	sprintf(buf, "Allocating ZMem %d bytes", size * cnt);
	LOG_INFO(buf);
#endif
	void* data = calloc(cnt, size);
	return data;
}

void ZMemFree(voidpf custom, voidpf data) {
#ifdef ZMEM_LOG
	LOG_INFO("Freeing ZMem");
#endif
	free(data);
}

int PackFileOpen(pack_file_t* pack_file, const char* filename) {
	char buf[1024];
	sprintf(buf, "Opening packed file \"%s\"", filename);
	LOG_INFO(buf);

	FILE* file = 0;
	size_t size = 0;

	memset(pack_file, 0x00, sizeof(pack_file_t));

	// Load all file into memory, data will be compressed an joined when writting
	// the pack file
	file = fopen(filename, "rb");
       
	if(0 == file) {
		sprintf(buf, "Error opening pack file \"%s\"", filename);
		LOG_ERROR(buf);
		return -1;
	}	
	
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);

	// Check file
	LOG_INFO("Checking packed file");
	if(size >= sizeof(pack_file_header_t)) {
		pack_file_header_t header;
		fread(&header, sizeof(pack_file_header_t), 1, file);
		
		LOG_INFO("Checking magic");
		// Check magic
		if(header.magic[0] == 'C' && header.magic[1] == 'A' &&
			header.magic[2] == 'F' && header.magic[3] == '0') {
			sprintf(buf, "Found %d entries", header.entry_count);
			LOG_INFO(buf);
			pack_file->entry_count = header.entry_count;
			
			uint8_t* entry_table = 0;
			pack_file->entries = calloc(sizeof(pack_file_entry_t), header.entry_count);

			if(0 == pack_file->entries) {
				LOG_ERROR("Failed allocating memory for entry table");
				fclose(file);
				return -2;
			}

			entry_table = calloc(header.entry_table_size, 1);

			if(0 == entry_table) {
				LOG_ERROR("Failed allocating memory for entry table (expanded)");
				fclose(file);
				return -3;
			}

			// Read all expanded table
			fread(entry_table, header.entry_table_size, 1, file);

			uint8_t* entry_table_data = entry_table;
			pack_file->file = file;
			pack_file->state = 1;
			pack_file->header = header;

			// Start filling the entry table on memory
			LOG_INFO("Entry table has:");
			for(uint64_t i = 0; i < header.entry_count; i++){
				size_t len = strlen(entry_table_data);
				pack_file_entry_t* entry = pack_file->entries + i;

				// Get a reference to the name
				entry->name = calloc(len + 1, 1);
				memcpy(entry->name, entry_table_data, len);
				entry_table_data += len + 1;

				// Fill int64 entries
				entry->offset = *(uint64_t*)entry_table_data;
				entry_table_data += sizeof(uint64_t);
				entry->output_size = *(uint64_t*)entry_table_data;
				entry_table_data += sizeof(uint64_t);
				entry->this_size = *(uint64_t*)entry_table_data;
				entry_table_data += sizeof(uint64_t);
				entry->checksum = *(uint64_t*)entry_table_data;
				entry_table_data += sizeof(uint64_t);
			}

			free(entry_table);
			for(uint64_t i = 0; i < header.entry_count; i++){
				pack_file_entry_t* entry = pack_file->entries + i;
				sprintf(buf, "Entry \"%s\" c %d u %d", entry->name, entry->this_size, entry->output_size);
				LOG_INFO(buf);
			}

		}
		else {
			sprintf(buf, "Incorrect magic %c%c%c%c (Should be CAF0)", 
					header.magic[0],
					header.magic[1],
					header.magic[2],
					header.magic[3]
					);
			LOG_ERROR(buf);
		}
		
	}
	else {
		// Not a complete file or corresponding
		LOG_ERROR("Not valid pack file");
	}

	return 0;
}

int PackFileClose(pack_file_t* pack_file) {
	LOG_INFO("Closing packed file");
	assert(0 != pack_file);
	
	// Files are loaded into memory
	LOG_INFO("Freeing entry data");
	for(uint64_t i = 0; i < pack_file->header.entry_count; i++) {
		pack_file_entry_t* entry = pack_file->entries + i;
		if(0 != entry->name) {
			free(entry->name);
			entry->name = 0;
		}
		if(0 != entry->data) {
			free(entry->data);
			entry->data = 0;
		}
	}
	
	// Delete loaded entry table (The one loaded from file)

	if(0 != pack_file->file) {
		LOG_INFO("Closing packed file");
		fclose(pack_file->file);
		pack_file->file = 0;
	}

	return 0;
}

int PackFileCreate(pack_file_t* pack_file){
	LOG_INFO("Creating packed file");
	assert(0 != pack_file);

	// Initialize a new pack file
	pack_file->file = 2; // Get ready for write
	pack_file->state = 0;
	pack_file->file_size = 0;
	pack_file->entry_count = 0;
	pack_file->entry_max = 0;
	pack_file->current_offset = 0;
	pack_file->header.magic[0] = 'C';
	pack_file->header.magic[1] = 'A';
	pack_file->header.magic[2] = 'F';
	pack_file->header.magic[3] = '0';
	pack_file->header.flags = 0;
	pack_file->header.entry_count = 0;
	pack_file->header.entry_table_size = 0;
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
		//for(int i = 0; i < 32; i++){
		//	printf("%2x ", data[i]);
		//}
		//printf("\n");
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
			// Fill entry
			pack_file_entry_t entry;
			entry.name = calloc(strlen(filename) + 1, 1);
			memcpy(entry.name, filename, strlen(filename));
			entry.loaded = 1;
			entry.offset = pack_file->current_offset;
			entry.output_size = size;
			entry.this_size = compressed_size;
			entry.checksum = 0;
			entry.data = data_out;
					
			free(data);
			// Increment offset
			pack_file->current_offset += compressed_size;

			//sprintf(buf, "Went from %lld -> %lld", size, compressed_size);
			//LOG_INFO(buf);

			// Add entry to table
			if(0 == PackFileDoAddEntry(pack_file, &entry)) {
				LOG_INFO("Success on adding entry");
			} else {
				LOG_ERROR("Failed adding entry");
			}
			return 0;

			// Decompress data to check integrity
			stream.zalloc = ZMemAlloc;
			stream.zfree = ZMemFree;
			stream.avail_in = 0;
			stream.next_in = 0;
			memset(data, 0, size);
			if(Z_OK == inflateInit(&stream)) {
				LOG_INFO("Begin inflate test");
				
				// Prepare full inflate
				stream.avail_in = compressed_size;
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

				//for(int i = 0; i < 32; i++){
				//	printf("%2x ", data[i]);
				//}
				//printf("\n");

				free(data);
				sprintf(buf, "Decompressed total of %d bytes", stream.total_out);	
				LOG_INFO(buf);
				if(Z_OK == result && stream.total_out == size) {
					LOG_INFO("Decompression success");

					// Fill entry
					pack_file_entry_t entry;
					entry.name = calloc(strlen(filename) + 1, 1);
					memcpy(entry.name, filename, strlen(filename));
					entry.loaded = 1;
					entry.offset = pack_file->current_offset;
					entry.output_size = size;
					entry.this_size = compressed_size;
					entry.checksum = 0;
					entry.data = data_out;
					

					// Increment offset
					pack_file->current_offset += compressed_size;

					//sprintf(buf, "Went from %lld -> %lld", size, compressed_size);
					//LOG_INFO(buf);

					// Add entry to table
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

int PackFileAddEntryFromMemory(pack_file_t* pack_file, char* data, size_t size, const char* entry_name){
	char buf[1024];
	sprintf(buf, "Adding entry with data from memory with name \"%s\"", entry_name);
	LOG_INFO(buf);
	assert(0 != data);

	if(0 == data) return -2; // Do not accept a nullptr
				 
	// Relevant structs
	z_stream stream;
	uint8_t* data_out = 0;

	sprintf(buf, "Entry data size: %lld", size);
	LOG_INFO(buf);
	

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
				LOG_INFO("Begin inflate test");
				
				// Prepare full inflate
				stream.avail_in = compressed_size;
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

				//for(int i = 0; i < 32; i++){
				//	printf("%2x ", data[i]);
				//}
				//printf("\n");

				sprintf(buf, "Decompressed total of %d bytes\n", stream.total_out);	
				LOG_INFO(buf);
				if(Z_OK == result && stream.total_out == size) {
					LOG_INFO("Decompression success");

					// Fill entry
					pack_file_entry_t entry;
					entry.name = calloc(strlen(entry_name) + 1, 1);
					memcpy(entry.name, entry_name, strlen(entry_name));
					entry.loaded = 1;
					entry.offset = pack_file->current_offset;
					entry.output_size = size;
					entry.this_size = compressed_size;
					entry.checksum = 0;
					entry.data = data_out;
					

					// Increment offset
					pack_file->current_offset += compressed_size;

					// Add entry to table
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

int PackFileWrite(pack_file_t* pack_file, const char* filename){
	char buf[1024];
	sprintf(buf, "Writting packed file \"%s\" with %d entries", filename, pack_file->entry_count);
	LOG_INFO(buf);
	pack_file->header.entry_count = pack_file->entry_count;
	FILE* output = 0;
	uint64_t offset = 0;

	output = fopen(filename, "wb");
	if(0 == output) {
		LOG_ERROR("Error writting pack file");
		return -1;
	}


	// Calculate entry table size
	for(size_t i = 0; i < pack_file->entry_count; i++) {
		pack_file->header.entry_table_size += strlen(pack_file->entries[i].name) + 1 + sizeof(uint64_t) * 4;
	}

	fwrite(&pack_file->header, sizeof(pack_file_header_t), 1, output);

	// Then write that table
	for(size_t i = 0; i < pack_file->entry_count; i++) {
		pack_file_entry_t* entry = pack_file->entries + i;
		sprintf(
			buf,
			"%s : %d -> %d bytes", 
			entry->name,
		       	entry->output_size,
			entry->this_size
			);

		LOG_INFO(buf);
		fwrite(
			entry->name,
			strlen(entry->name),
		       	1,
		       	output
			);
		fputc(0x00, output);
		fwrite(&offset, sizeof(uint64_t), 1, output);
		fwrite(&entry->output_size, sizeof(uint64_t), 1, output);
		fwrite(&entry->this_size, sizeof(uint64_t), 1, output);
		fwrite(&entry->checksum, sizeof(uint64_t), 1, output);
		offset += entry->this_size;
	}

	for(size_t i = 0; i < pack_file->entry_count; i++) {
		pack_file_entry_t* entry = pack_file->entries + i;
		fwrite(entry->data, sizeof(uint8_t), entry->this_size, output);
	}
	LOG_INFO("Finished writting packed file");
	fclose(output);


	return 0;
}

int PackFileLoadEntry(pack_file_t* pack_file, const char* entry_name, void** data, size_t* size) {
	char buf[1024];
	sprintf(buf, "Loading packed file entry \"%s\"", entry_name);
	LOG_INFO(buf);

	assert(0 != pack_file);
	assert(0 != pack_file->file);
	assert(0 != data);
	assert(0 != size);

	if(0 == data || 0 == size) {
		return -1;
	}

	z_stream stream;
	int result = 0;
	pack_file_entry_t* entry = 0;
	uint8_t* data_in = 0;
	
	if(0 == PackFileFindAndGetEntry(pack_file, entry_name, &entry)) {
		sprintf(buf, "Entry \"%s\" not found", entry_name);
		LOG_ERROR(buf);
		return -2;
	}

	if(0 != entry->data) {
		*data = entry->data;
		*size = entry->output_size;
		return 0;
	}

	LOG_INFO("Retrieving entry data from file");
	data_in = calloc(entry->this_size, 1);
	fseek(pack_file->file, sizeof(pack_file_header_t) + pack_file->header.entry_table_size + entry->offset, SEEK_SET);
	fread(data_in, entry->this_size, 1, pack_file->file);

	// Decompress data
	stream.data_type = Z_BINARY;
	stream.zalloc = ZMemAlloc;
	stream.zfree = ZMemFree;
	stream.avail_in = 0;
	stream.next_in = 0;
	if(Z_OK == inflateInit(&stream)) {
		LOG_INFO("Inflating data");
		
		uint8_t* data_out = calloc(entry->output_size + 1, 1);

		// Prepare full inflate
		stream.avail_in = entry->this_size;
		stream.next_in = data_in;
		stream.avail_out = entry->output_size;
		stream.next_out = data_out;
		result = inflate(&stream, Z_FINISH);
		
		free(data_in);

		if(Z_STREAM_END == result) {
			LOG_INFO("Inflate success");
		}
		else {
			sprintf(buf, "Inflate failed: ID %d MSG %s", result, stream.msg);
			LOG_ERROR(buf);
		}

		result = inflateEnd(&stream);

		//for(int i = 0; i < 32; i++){
		//	printf("%2x ", ((uint8_t*)data_out)[i]);
		//}
		//printf("\n");

		sprintf(buf, "Decompressed total of %d bytes", stream.total_out);	
		LOG_INFO(buf);
		if(Z_OK == result && stream.total_out == entry->output_size) {
			LOG_INFO("Decompression success");
			entry->data = data_out;
			*data = data_out;
			*size = entry->output_size;
		}
		else {
			sprintf(buf, "An error ocurred while finishing inflate: ID: %d MSG: %s",  result, stream.msg);
			LOG_ERROR(buf);

		}
	}
	return 0;
}

int PackFileFindEntry(pack_file_t* pack_file, const char* entry_name) {
	char buf[1024];
	sprintf(buf, "Looking for file entry \"%s\"", entry_name);
	LOG_INFO(buf);

	// Iterate and find same name
	uint64_t entries = pack_file->header.entry_count;
	for (int64_t i = 0; i < entries; i++) {
		if(0 == (strcmp(entry_name, pack_file->entries[i].name))){
			return 1;
		}
	}

	return 0;
}

int PackFileFindAndGetEntry(pack_file_t* pack_file, const char* entry_name, pack_file_entry_t** entry) {
	char buf[512];
	sprintf(buf, "Getting entry \"%s\" data", entry_name);
	LOG_INFO(buf);

	assert(0 != entry);
	if(0 == entry) {
		return 0;
	}
	
	// Iterate and find same name
	uint64_t entries = pack_file->entry_count;
	for (int64_t i = 0; i < entries; i++) {
		if(0 == (strcmp(entry_name, pack_file->entries[i].name))){
			*entry = pack_file->entries + i;
			return 1;
		}
	}	
	return 0;
}

int PackFileDoAddEntry(pack_file_t* pack_file, pack_file_entry_t* entry) {
	char buf[512];
	LOG_INFO("Adding entry to pack file");
	pack_file_entry_t* find = 0;
	if(1 == PackFileFindAndGetEntry(pack_file, entry->name, &find)) {
		sprintf(buf, "Replacing data on entry \"%s\"", entry->name);
		LOG_INFO(buf);

		if(0 != find->data) {
			free(find->data);
		}
		free(entry->name);
		find->data = entry->data;
		find->this_size = entry->this_size;
		find->output_size = entry->output_size;
	} 
	else {
		if (pack_file->entry_max > 0 && pack_file->entry_count < pack_file->entry_max) {
			pack_file->entries[pack_file->entry_count] = *entry;
			//sprintf(buf, "Integrity: s %lld d %lld", entry->this_size, pack_file->entries[pack_file->entry_count].this_size);
			//LOG_INFO(buf);
			pack_file->entry_count++;
		} else if (pack_file->entry_max <= 0) {
			pack_file->entries = calloc(4, sizeof(pack_file_entry_t));
			pack_file->entry_max = 4;
			pack_file->entry_count = 1;
			pack_file->entries[0] = *entry;
			//sprintf(buf, "Integrity: s %lld d %lld", entry->this_size, pack_file->entries[0].this_size);
			//LOG_INFO(buf);
		} else if (pack_file->entry_count >= pack_file->entry_max) {
			pack_file->entry_max *= 2;
			pack_file->entries = realloc(pack_file->entries, sizeof(pack_file_entry_t) * pack_file->entry_max);
			pack_file->entries[pack_file->entry_count] = *entry;
			//sprintf(buf, "Integrity: s %lld d %lld", entry->this_size, pack_file->entries[pack_file->entry_count].this_size);
			//LOG_INFO(buf);
			pack_file->entry_count++;
		}
	}

	return 0;
}
