/********************************************\
 pl32lib-ng, v1.06
 (c) 2022 pocketlinux32, Under MPL v2.0
 pl32-file.h: File management module header
\********************************************/
#pragma once
#include <pl32-memory.h>

typedef struct plfile plfile_t;

plfile_t* plFOpen(string_t filename, string_t mode, plmt_t* mt);
plfile_t* plFToP(FILE* pointer, string_t mode, plmt_t* mt);
int plFClose(plfile_t* ptr);

size_t plFRead(memptr_t ptr, size_t size, size_t nmemb, plfile_t* stream);
size_t plFWrite(memptr_t ptr, size_t size, size_t nmemb, plfile_t* stream);

int plFPutC(byte_t ch, plfile_t* stream);
int plFGetC(plfile_t* stream);

int plFPuts(string_t string, plfile_t* stream);
string_t plFGets(string_t string, int num, plfile_t* stream);

int plFSeek(plfile_t* stream, long int offset, int whence);
size_t plFTell(plfile_t* stream);

int plFPToFile(string_t filename, plfile_t* stream);
void plFCat(plfile_t* dest, plfile_t* src, int destWhence, int srcWhence, bool closeSrc);
