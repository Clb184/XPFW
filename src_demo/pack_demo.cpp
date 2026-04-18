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
			if(0 == PackFileAddEntryFromFile(&pack_file, argv[i])) {
				printf("Entry \"%s\" added successfuly\n", argv[i]);
			}
		}
		if(0 == PackFileWrite(&pack_file, "test.dat")) {
			printf("Packed file written successfuly\n");
			if(0 == PackFileOpen(&pack_file, "test.dat")){
				printf("Packed file opened successfuly\n");
			}
		}
	}
	return 0;
}
