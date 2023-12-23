/**************************************\
 pl-rt, v1.00
 (c) 2023 pocketlinux32, Under MPL v2.0
 plrt-posix.h: POSIX module
\**************************************/

#include <plrt-posix.h>
#include <errno.h>

extern void signalHandler(int signal);

void plRTSetSignal(int signal){
	struct sigaction oldHandler;
	struct sigaction newHandler;

	newHandler.sa_handler = signalHandler;
	sigemptyset(&newHandler.sa_mask);
	newHandler.sa_flags = 0;

	sigaction(signal, NULL, &oldHandler);
	if(oldHandler.sa_handler != SIG_IGN)
		sigaction(signal, &newHandler, NULL);
}

int plRTSpawn(plptr_t args){
	pid_t exec = fork();
	if(exec == 0){
		sleep(1);
		char* rawArgs[args.size + 1];
		for(int i = 0; i < args.size; i++)
			rawArgs[i] = ((plstring_t*)args.pointer)[i].data.pointer;
		rawArgs[args.size] = NULL;

		char buffer[256];
		execv(realpath(rawArgs[0], buffer), rawArgs);

		plRTPanic("plRTSpawn", PLRT_ERROR | PLRT_ERRNO | errno, false);
	}
	return exec;
}


int plRTSortDirents(const void* string1, const void* string2){
	return plRTStrcmpForAlphaSort(*(plstring_t*)string1, *(plstring_t*)string2);
}

plptr_t plRTGetDirents(char* path, plmt_t* mt){
	DIR* directory = opendir(path);
	struct dirent* directoryEntry;
	plptr_t fileList = {
		.pointer = plMTAlloc(mt, 2 * sizeof(plstring_t)),
		.size = 0
	};

	while((directoryEntry = readdir(directory)) != NULL){
		// Check to remove . and .. from directory listing
		if(strcmp(directoryEntry->d_name, ".") != 0 && strcmp(directoryEntry->d_name, "..") != 0){
			if(fileList.size > 1){
				memptr_t tempPtr = plMTRealloc(mt, fileList.pointer, (fileList.size + 1) * sizeof(plstring_t));
				if(tempPtr == NULL)
					plRTPanic("plRTGetDirents", PLRT_FAILED_ALLOC, false);

				fileList.pointer = tempPtr;
			}

			((plstring_t*)fileList.pointer)[fileList.size] = plRTStrFromCStr(directoryEntry->d_name, mt);
			fileList.size++;
		}
	}

	qsort(fileList.pointer, fileList.size, sizeof(plstring_t), plRTSortDirents);

	return fileList;
}


plfile_t* plRTLogStart(char* prefix, plmt_t* mt){
	char path[4096] = "/var/log";
	char filename[256] = "";
	struct stat dirExist;

	if(getuid() != 0){
		char* homePath = getenv("HOME");
		strcpy(path, homePath);
		strcat(path, "/.cache/");
		if(stat(path, &dirExist) == -1)
			mkdir(path, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	}

	snprintf(filename, 256, "/%ld.log", time(NULL));
	if(prefix != NULL){
		strcat(path, "/");
		strcat(path, prefix);
	}

	if(stat(path, &dirExist) == -1)
		mkdir(path, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	strcat(path, filename);

	plfile_t* retFile = plFOpen(path, "a", mt);

	plFPuts(plRTStrFromCStr("[INFO]: PortaLinux Logger, Version 1.00. (c)2023 pocketlinux32, under MPL 2.0\n", NULL), retFile);
	plFPuts(plRTStrFromCStr("[INFO]: Log file located in ", NULL), retFile);
	plFPuts(plRTStrFromCStr(path, NULL), retFile);
	plFPuts(plRTStrFromCStr("\n", NULL), retFile);
	return retFile;
}

void plRTLog(plfile_t* logFile, plloglevel_t logLevel, plstring_t string){
	plptr_t holderPtr = string.data;
	switch(logLevel){
		case LOG_DEBUG: ;
			string.data.pointer = "[DEBUG]: ";
			string.data.size = 9;
			break;
		case LOG_INFO: ;
			string.data.pointer = "[INFO]: ";
			string.data.size = 8;
			break;
		case LOG_WARNING: ;
			string.data.pointer = "[WARNING]: ";
			string.data.size = 12;
			break;
		case LOG_ERROR: ;
			string.data.pointer = "[ERROR]: ";
			string.data.size = 9;
			break;
		case LOG_FATAL_ERROR: ;
			string.data.pointer = "[FATAL]: ";
			string.data.size = 9;
			break;
	}

	plFPuts(string, logFile);
	string.data = holderPtr;
	plFPuts(string, logFile);
	plFPuts(plRTStrFromCStr("\n", NULL), logFile);
}

void plRTLogStop(plfile_t* logFile){
	plFPuts(plRTStrFromCStr("[INFO] Shutting down logger\n", NULL), logFile);
	plFClose(logFile);
}
