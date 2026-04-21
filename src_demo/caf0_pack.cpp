#include "XPFW.h"

#include <cstdio>
#include <cstdlib>

#ifdef WIN32
#include <windows.h>

void PackFilesWin32(int argc, char** argv) {
	HANDLE hFile = NULL;
	WIN32_FIND_DATAA find_data;
	BOOL cont = FALSE;

	pack_file_t pack_file;
	PackFileCreate(&pack_file);
	for(int i = 2; i < argc; i++){
		hFile = FindFirstFileA(argv[i], &find_data);
		if(INVALID_HANDLE_VALUE != hFile) {
			if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				printf("File \"%s\"\n", find_data.cFileName);
			}
			PackFileAddEntryFromFile(&pack_file, find_data.cFileName);
			do {
				cont = FindNextFileA(hFile, &find_data);
				if(TRUE == cont) { 
					if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						printf("File \"%s\"\n", find_data.cFileName);
					}
					PackFileAddEntryFromFile(&pack_file, find_data.cFileName);
				}
			} while(FALSE != cont);
			FindClose(hFile);
		}
	}
	PackFileWrite(&pack_file, argv[1]);
}
#endif

int main(int argc, char** argv) {
	if(argc < 3) {
		printf("Usage: caf0_pack <output_name> <file> ...\n");
		return -1;
	}
#ifdef WIN32
	PackFilesWin32(argc, argv);
#elif defined __linux__
	PackFilesLinux(argc, argv);
#endif

	return 0;
}
