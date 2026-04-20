#ifndef PACKFILEHEADER_INCLUDED
#define PACKFILEHEADER_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
// Entry for pack file header, is followed by name, output size, compressed size, maybe a checksum, and finally, data
typedef struct {
	char* name;
	int loaded;
	uint64_t offset;
	uint64_t output_size;
	uint64_t this_size;
	uint64_t checksum;
	char* data;
} pack_file_entry_t;

// Header for pack file, magic will be "CAF0" this time, then there will be a checksum, followed by the number of entries
// after this header, there will be an entry table
typedef struct {
	char magic[4]; // CAF0
	uint32_t flags; // flags, 0x1 means compressed
	uint64_t entry_count;
	uint64_t entry_table_size;
} pack_file_header_t;

#ifdef __cplusplus
}
#endif

#endif
