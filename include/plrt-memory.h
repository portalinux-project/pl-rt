/***************************************************\
 pl-rt, v0.01
 (c) 2023 pocketlinux32, Under MPL v2.0
 plrt-memory.h: Safe memory management module header
\***************************************************/
#pragma once
#if defined(__cplusplus) && !defined(PLRTCPP)
	#error Direct C header include in C++ detected. Please use plrt.hpp instead of directly using the C headers
#endif

#ifndef __cplusplus
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#endif

#define PLRT_VERSION "0.01"
#define PLRT_API_VER 0
#define PLRT_FEATURELVL 1
#define PLRT_PATCHLVL 0

typedef enum plmtaction {
	PLMT_GET_USEDMEM,
	PLMT_GET_MAXMEM,
	PLMT_SET_MAXMEM
} plmtaction_t;

typedef uint8_t byte_t;
typedef void* memptr_t;
typedef struct plmt plmt_t;

typedef struct plptr {
	memptr_t pointer;
	size_t size;
} plptr_t;

plmt_t* plMTInit(size_t maxMemoryAlloc);
void plMTStop(plmt_t* mt);
size_t plMTMemAmnt(plmt_t* mt, plmtaction_t action, size_t size);

memptr_t plMTAlloc(plmt_t* mt, size_t size);
memptr_t plMTAllocE(plmt_t* mt, size_t size);
memptr_t plMTCalloc(plmt_t* mt, size_t amount, size_t size);
memptr_t plMTRealloc(plmt_t* mt, memptr_t pointer, size_t size);
void plMTFree(plmt_t* mt, memptr_t pointer);
