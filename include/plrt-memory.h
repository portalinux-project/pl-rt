/***************************************************\
 pl-rt, v1.02
 (c) 2024 CinnamonWolfy, Under MPL v2.0
 plrt-memory.h: Safe memory management module header
\***************************************************/

#pragma once
#ifndef __cplusplus
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif
#include <plrt-types.h>

typedef struct plmt plmt_t;
typedef enum plmtaction {
	PLMT_GET_USEDMEM,
	PLMT_GET_MAXMEM,
	PLMT_SET_MAXMEM
} plmtaction_t;

plmt_t* plMTInit(size_t maxMemoryAlloc);
void plMTStop(plmt_t* mt);
size_t plMTMemAmnt(plmt_t* mt, plmtaction_t action, size_t size);

memptr_t plMTAlloc(plmt_t* mt, size_t size);
memptr_t plMTRealloc(plmt_t* mt, memptr_t pointer, size_t size);
void plMTFree(plmt_t* mt, memptr_t pointer);
