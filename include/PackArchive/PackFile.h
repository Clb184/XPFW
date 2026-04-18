#ifndef PACKFILE_INCLUDED
#define PACKFILE_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "PFHeader.h"

#ifdef __cplusplus
extern "C" {
#endif
	
// Pass a pack_file_t struct to realize many operations like write a new packfile, open a packfile,
// retrieve file from packfile, etc

typedef struct {
	FILE* file;
	size_t file_size;
	size_t entry_count;
	size_t entry_max;
	pack_file_header_t header;
	pack_file_entry_t* entries;
} pack_file_t;

// Pack file open, pass structure and it will be filled if a file can be opened
// return 0 means success
int PackFileOpen(pack_file_t* pack_file, const char* filename);

// Create the pack file structure and prepare for writting
int PackFileCreate(pack_file_t* pack_file);

// Add an entry by loading a file
int PackFileAddEntryFromFile(pack_file_t* pack_file, const char* filename);

// Add an entry by loading data from memory
int PackFileAddEntryFromMemory(pack_file_t* pack_file, char* data, const char* filename);

// Write a packed file 
int PackFileWrite(pack_file_t* pack_file, const char* filename);

// Retrieve data from entry
int PackFileLoadEntry(pack_file_t* pack_file, const char* entry_name, char** data, size_t* size);

int PackFileFindEntry(pack_file_t* pack_file, const char* entry_name, pack_file_entry_t** out);

int PackFileDoAddEntry(pack_file_t* pack_file, pack_file_entry_t* entry);
#ifdef __cplusplus
}
#endif

#endif
