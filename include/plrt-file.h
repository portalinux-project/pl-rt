/******************************************\
 pl-rt, v1.04
 (c) 2024 CinnamonWolfy, Under MPL v2.0
 plrt-file.h: File management module header
\******************************************/

#pragma once
#include <plrt-memory.h>
#include <plrt-string.h>

typedef struct plfile plfile_t;

plfile_t* plFOpen(char* filename, char* mode, plmt_t* mt);
plfile_t* plFToP(FILE* pointer, plmt_t* mt);
int plFClose(plfile_t* ptr);

size_t plFRead(plptr_t ptr, plfile_t* stream);
size_t plFWrite(plptr_t ptr, plfile_t* stream);

int plFPutC(byte_t ch, plfile_t* stream);
int plFGetC(plfile_t* stream);

int plFPuts(plstring_t string, plfile_t* stream);
int plFGets(plstring_t* string, plfile_t* stream);

#ifdef PLRT_COMPAT_FEATURELVL
#if PLRT_COMPAT_FEATURELVL < 3
int plFPutsOld(plstring_t string, plfile_t* stream){
	return !plFPuts(string, stream);
}

int plFGetsOld(plstring_t* string, plfile_t* stream){
	return !plFGets(string, stream);
}
#define plFGets plFGetsOld
#define plFPuts plFPutsOld
#endif
#endif

int plFSeek(plfile_t* stream, long int offset, int whence);
size_t plFTell(plfile_t* stream);
void plFFlush(plfile_t* stream);

void plFLoad(plfile_t* stream);
void plUnload(plfile_t* stream);
