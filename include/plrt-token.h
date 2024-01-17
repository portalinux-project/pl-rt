/**********************************************************\
 pl-rt, v1.00
 (c) 2022 pocketlinux32, Under MPL v2.0
 plrt-token.h: String manipulation and parser module header
\**********************************************************/
#pragma once
#include <plrt-string.h>
#include <plrt-memory.h>

plstring_t plRTTokenize(plstring_t string, plstring_t* leftoverStr, plmt_t* mt);
plptr_t plRTParser(plstring_t input, plmt_t* mt);
void plRTFreeParsedString(plptr_t stringArray);
void plRTExtractContents(plstring_t* string);
