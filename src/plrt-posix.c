/**************************************\
 pl-rt, v1.00
 (c) 2023 pocketlinux32, Under MPL v2.0
 plrt-posix.h: POSIX module
\**************************************/

#include <plrt-log.h>

plfile_t* plRTLogStart(char* prefix, plmt_t* mt){
	char path[4096] = "/var/log";
	char filename[256] = "";

	snprintf(filename, 256, "%d.log", prefix, time());
	if(prefix != NULL)
		strcat(path, prefix);
	strcat(path, filename);

	plfile_t* retFile = plFOpen(path, "a", mt);

	plFPuts(retFile, plRTStrFromCStr("[INFO]: PortaLinux Logger, Version 1.00. (c)2023 pocketlinux32, under MPL 2.0\n", NULL));
}

void plRTLog(pllogfile_t* logFile, plloglevel_t logLevel, plstring_t string){
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
	}

	plRT
}
