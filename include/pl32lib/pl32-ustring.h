/**********************************************\
 pl32lib-ng, v1.06
 (c) 2022-2023 pocketlinux32, Under MPL v2.0
 pl32-ustring.h: UTF-8 String ops module header
\**********************************************/
#pragma once
#include <pl32-memory.h>

typedef struct plchar {
	byte_t bytes[4];
} plchar_t;

typedef struct plstring {
	plptr_t data;
	plmt_t* mt
	bool isplChar;
} plstring_t;

plstring_t plUStrFromCStr(char* cStr, plmt_t* mt);
void plUStrCompress(plstring_t* plCharStr, plmt_t* mt);
memptr_t plMemMatch(plarray_t* memBlock1, plarray_t* memBlock2);
int64_t plUStrchr(plstring_t* string, plchar_t chr, size_t startAt);
int64_t plUStrstr(plstring_t* string1, plstring_t* string2, size_t startAt);
plstring_t plUStrtok(plstring_t* string, plstring_t* delimiter, plstring_t* leftoverStr, plmt_t* mt);
plstring_t plUStrdup(plstring_t* string, bool compress, plmt_t* mt);
