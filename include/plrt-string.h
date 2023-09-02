/**********************************************\
 pl-rt, v0.04
 (c) 2022-2023 pocketlinux32, Under MPL v2.0
 plrt-string.h: UTF-8 String ops module header
\**********************************************/
#pragma once
#include <plrt-memory.h>

typedef struct plchar {
	byte_t bytes[4];
} plchar_t;

typedef struct plstring {
	plptr_t data;
	plmt_t* mt;
	bool isplChar;
} plstring_t;

plstring_t plRTStrFromCStr(char* cStr, plmt_t* mt);
void plRTStrCompress(plstring_t* plCharStr, plmt_t* mt);
memptr_t plRTMemMatch(plptr_t* memBlock1, plptr_t* memBlock2);
int64_t plRTStrchr(plstring_t* string, plchar_t chr, size_t startAt);
int64_t plRTStrstr(plstring_t* string1, plstring_t* string2, size_t startAt);
plstring_t plRTStrtok(plstring_t* string, plstring_t* delimiter, plstring_t* leftoverStr, plmt_t* mt);
plstring_t plRTStrdup(plstring_t* string, bool compress, plmt_t* mt);
