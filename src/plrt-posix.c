/**************************************\
 pl-rt, v1.04
 (c) 2024 CinnamonWolfy, Under MPL v2.0
 plrt-posix.c: POSIX module
\**************************************/

#define PLRT_ENABLE_HANDLER
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
	if(args.pointer == NULL)
		plRTPanic("plRTSpawn", PLRT_ERROR | PLRT_NULL_PTR, true);

	char* rawArgs[args.size + 1];
	for(int i = 0; i < args.size; i++)
		rawArgs[i] = ((plstring_t*)args.pointer)[i].data.pointer;
	rawArgs[args.size] = NULL;

	char buffer[256];
	pid_t exec = fork();
	if(exec == 0){
		struct timespec sleepconst = {
			.tv_sec = 0,
			.tv_nsec = 1000
		};
		nanosleep(&sleepconst, NULL);
		execv(realpath(rawArgs[0], buffer), rawArgs);

		plRTPanic("plRTSpawn", PLRT_ERROR | PLRT_ERRNO | errno, false);
	}
	return exec;
}


plptr_t plRTGetDirents(char* path, plmt_t* mt){
	if(path == NULL || mt == NULL)
		plRTPanic("plRTGetDirents", PLRT_ERROR | PLRT_NULL_PTR, true);

	DIR* directory = opendir(path);
	struct dirent* directoryEntry;
	plptr_t fileList = {
		.pointer = plMTAlloc(mt, 2 * sizeof(struct dirent)),
		.size = 0
	};

	while((directoryEntry = readdir(directory)) != NULL){
		if(fileList.size > 1){
			memptr_t tempPtr = plMTRealloc(mt, fileList.pointer, (fileList.size + 1) * sizeof(struct dirent));
			if(tempPtr == NULL)
				plRTPanic("plRTGetDirents", PLRT_FAILED_ALLOC, false);

			fileList.pointer = tempPtr;
		}

		((struct dirent*)fileList.pointer)[fileList.size] = *directoryEntry;
		fileList.size++;
	}

	return fileList;
}

int internalSorter(const void* dirent1, const void* dirent2){
	return strcmp(((struct dirent*)dirent1)->d_name, ((struct dirent*)dirent2)->d_name);
}

void plRTSortDirents(plptr_t direntArray){
	if(direntArray.pointer == NULL)
		plRTPanic("plRTSortDirents", PLRT_ERROR | PLRT_NULL_PTR, true);

	qsort(direntArray.pointer, direntArray.size, sizeof(struct dirent), internalSorter);
}

plfile_t* plRTLogStart(char* prefix, plmt_t* mt){
	if(mt == NULL)
		plRTPanic("plRTLogStart", PLRT_ERROR | PLRT_NULL_PTR, true);

	char path[4096] = "/var/log";
	char filename[256] = "";
	struct stat dirExist;

	if(getuid() != 0){
		char* homePath = getenv("HOME");
		strcpy(path, homePath);
		strcat(path, "/.cache");
		if(stat(path, &dirExist) == -1)
			mkdir(path, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	}

	struct timespec buffer;
	clock_gettime(CLOCK_REALTIME, &buffer);
	snprintf(filename, 256, "/%ld.log", buffer.tv_sec);
	if(prefix != NULL){
		strcat(path, "/");
		strcat(path, prefix);
	}

	if(stat(path, &dirExist) == -1)
		mkdir(path, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	strcat(path, filename);

	plfile_t* retFile = plFOpen(path, "a", mt);

	plFPuts(plRTStrFromCStr("[INFO]: PortaLinux Logger, Version 1.04. (c)2024 CinnamonWolfy, under MPL 2.0\n", NULL), retFile);
	plFPuts(plRTStrFromCStr("[INFO]: Log file located in ", NULL), retFile);
	plFPuts(plRTStrFromCStr(path, NULL), retFile);
	plFPuts(plRTStrFromCStr("\n", NULL), retFile);
	plFFlush(retFile);
	return retFile;
}

void plRTLog(plfile_t* logFile, plloglevel_t logLevel, plstring_t string){
	if(logFile == NULL || string.data.pointer == NULL)
		plRTPanic("plRTLog", PLRT_ERROR | PLRT_NULL_PTR, true);

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
	plFFlush(logFile);
}

void plRTLogStop(plfile_t* logFile){
	if(logFile == NULL)
		plRTPanic("plRTLogStop", PLRT_ERROR | PLRT_NULL_PTR, true);

	plFPuts(plRTStrFromCStr("[INFO] Shutting down logger\n", NULL), logFile);
	plFClose(logFile);
}
