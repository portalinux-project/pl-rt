/*****************************************************************\
 pl32lib-ng, v1.06
 (c) 2022 pocketlinux32, Under MPL v2.0
 pl32-token.h: String manipulation and parser module header
\*****************************************************************/
#pragma once
#include <pl32-ustring.h>
#include <pl32-memory.h>

//#ifdef PL32LIB_ENABLE_OLD_STRTOK
//	#pragma message("This function is deprecated, and will be fully removed when breaking changes are committed")
	string_t plStrtok(string_t string, string_t delimiter, string_t* leftoverStr, plmt_t* mt);
//#endif

string_t plTokenize(string_t string, string_t* leftoverStr, plmt_t* mt);
plarray_t* plParser(string_t input, plmt_t* mt);

