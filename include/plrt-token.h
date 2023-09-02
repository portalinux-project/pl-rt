/*****************************************************************\
 pl-rt, v0.04
 (c) 2022 pocketlinux32, Under MPL v2.0
 plrt-token.h: String manipulation and parser module header
\*****************************************************************/
#pragma once
#include <plrt-string.h>
#include <plrt-memory.h>

string_t plRTTokenize(plstring_t string, plstring_t* leftoverStr, plmt_t* mt);
plarray_t* plRTParser(plstring_t input, plmt_t* mt);

