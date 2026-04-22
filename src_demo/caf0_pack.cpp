#include "XPFW.h"

#include <cstdio>
#include <cstdlib>

#ifdef WIN32
#include <windows.h>

void RemoveFileNameWin32(char* path) {
	int len = strlen(path);
	printf("len: %lld\n", len);
	for(int i = len -1; i >= 0; i--) {
		if('\\' == path[i] || '/' == path[i]) break;
		path[i] = 0x00;
	}
}

void PackFilesWin32(int argc, char** argv) {
	HANDLE hFile = NULL;
	WIN32_FIND_DATAA find_data;
	BOOL cont = FALSE;
	char buf[2048];
	char parent_path[1024];

	pack_file_t pack_file;
	PackFileCreate(&pack_file);
	for(int i = 2; i < argc; i++){
		hFile = FindFirstFileA(argv[i], &find_data);
		if(INVALID_HANDLE_VALUE != hFile) {
			memset(parent_path, 0x00, 1024);
			sprintf(parent_path, "%s", argv[i]);
			printf("buf: %s\n", parent_path);
			RemoveFileNameWin32(parent_path);
			printf("Path with no filename: %s\n", parent_path);
			if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				sprintf(buf, "%s%s", parent_path, find_data.cFileName);
				printf("File \"%s\"\n", buf);
			}
			PackFileAddEntryFromFile(&pack_file, buf);
			do {
				cont = FindNextFileA(hFile, &find_data);
				if(TRUE == cont) { 
					if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						sprintf(buf, "%s%s", parent_path, find_data.cFileName);
						printf("File \"%s\"\n", buf);
					}
					PackFileAddEntryFromFile(&pack_file, buf);
				}
			} while(FALSE != cont);
			FindClose(hFile);
		}
	}
	PackFileWrite(&pack_file, argv[1]);
}

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
