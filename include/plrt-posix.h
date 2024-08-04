/**************************************\
 pl-rt, v1.04
 (c) 2024 CinnamonWolfy, Under MPL v2.0
 plrt-posix.h: POSIX module header
\**************************************/

#pragma once
#define _XOPEN_SOURCE 700
#include <plrt-file.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

typedef enum plloglevel {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FATAL_ERROR
} plloglevel_t;

#ifdef PLRT_ENABLE_HANDLER
void plRTSetSignal(int signal);
#else
void signalHandler(int signal){}
#endif
pid_t plRTSpawn(plptr_t args);
plptr_t plRTGetDirents(char* path, plmt_t* mt);
void plRTSortDirents(plptr_t direntArray);
#ifdef  PLRT_COMPAT_FEATURELVL
#if PLRT_COMPAT_FEATURELVL < 4
plptr_t plRTGetDirentNames(char* path, plmt_t* mt){
	plptr_t workPtr = plRTGetDirents(path, mt);
	plptr_t retPtr = {
		.pointer = plMTAlloc(mt, workPtr.size * sizeof(plstring_t)),
		.size = workPtr.size
	};

	plRTSortDirents(workPtr);
	for(int i = 0; i < retPtr.size; i++)
		((plstring_t*)retPtr.pointer)[i] = plRTStrFromCStr(((struct dirent*)workPtr.pointer)[i].d_name, mt);

	return retPtr;
}
#define plRTGetDirents plRTGetDirentNames
#endif
#endif

plfile_t* plRTLogStart(char* prefix, plmt_t* mt);
void plRTLog(plfile_t* logFile, plloglevel_t logLevel, plstring_t string);
void plRTLogStop(plfile_t* logFile);
