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
				char buf[1024];
				for(int i = 1; i < argc; i++){
					char* data = nullptr;
					size_t size = 0;

					PackFileLoadEntry(&pack_file, argv[i], (void**)&data, &size);

					sprintf(buf, "%s.dat", argv[i]);
					FILE* fp = fopen(buf, "wb");
					fwrite(data, size, 1, fp);
					fclose(fp);
					printf("Writting DAT file %s\n", buf);
				}
				char* data2 = nullptr;
				size_t sz2 = 0;
				PackFileLoadEntry(&pack_file, "null_data", (void**)&data2, &sz2);
				if(0 == PackFileClose(&pack_file)) {
					printf("Packed file closed successfuly\n");
				}
			}
		}
	}
	return 0;
}
