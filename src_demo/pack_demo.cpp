#include "XPFW.h"
#include "PackArchive/PackArchive.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char** argv) {
	// if arguments are insuficient, don't proceed
	if(argc < 2) {
		printf("Usage: pack_demo <file> ...\n");
		return -1;
	}
	
	// Test subject
	pack_file_t pack_file;	
	if(0 == PackFileCreate(&pack_file)) {
		for(int i = 1; i < argc; i++) {
			PackFileAddEntryFromFile(&pack_file, argv[i]);
		}
		PackFileWrite(&pack_file, "test.dat");
	}
	return 0;
}
