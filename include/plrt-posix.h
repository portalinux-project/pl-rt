/**************************************\
 pl-rt, v1.00
 (c) 2023 pocketlinux32, Under MPL v2.0
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

void plRTSetSignal(int signal);
pid_t plRTSpawn(plptr_t args);
plptr_t plRTGetDirents(char* path, plmt_t* mt);

plfile_t* plRTLogStart(char* prefix, plmt_t* mt);
void plRTLog(plfile_t* logFile, plloglevel_t logLevel, plstring_t string);
void plRTLogStop(plfile_t* logFile);
