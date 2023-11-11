/*******************************************\
 pl-rt, v0.06.1
 (c) 2022-2023 pocketlinux32, Under MPL v2.0
 plrt-plml.h: PLML parser module header
\*******************************************/
#pragma once
#include <plrt-token.h>

typedef enum plmltype {
	PLML_TYPE_NULL,
	PLML_TYPE_INT,
	PLML_TYPE_BOOL,
	PLML_TYPE_FLOAT,
	PLML_TYPE_STRING,
	PLML_TYPE_HEADER
} plmltype_t;

typedef union plmlval {
	long integer;
	bool boolean;
	double decimal;
	plptr_t string;
	plptr_t array;
} plmlval_t;

typedef struct plmltoken {
	plstring_t name;
	plmltype_t type;
	plmlval_t value;
	bool isArray;
	const plmt_t* const mt;
} plmltoken_t;

plmltoken_t plMLParse(plstring_t string, plmt_t* mt);
void plMLFreeToken(plmltoken_t token);

