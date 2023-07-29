/**********************************************************\
 pl-rt, v0.01
 (c) 2023 pocketlinux32, Under MPL v2.0
 plrt-types.h: Types header and Error handler module header
\**********************************************************/

#pragma once
#ifndef __cplusplus
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#endif

#define PLRT_VERSION "0.01"
#define PLRT_API_VER 0
#define PLRT_FEATURELVL 1
#define PLRT_PATCHLVL 0
#define UINT_SHIFT_LIMIT 63

typedef uint8_t byte_t;
typedef void* memptr_t;

typedef struct plptr {
	memptr_t pointer;
	size_t size;
} plptr_t;

typedef enum plrtret {
	PLRT_ERROR = ((uint64_t)1 << UINT_SHIFT_LIMIT),
	PLRT_ERRNO = ((uint64_t)1 << (UINT_SHIFT_LIMIT - 1)),

	PLRT_OOM = 1,
	PLRT_FAILED_ALLOC = 2,
	PLRT_INVALID_PTR = 3,
	PLRT_NULL_PTR = 4,

	PLRT_ACCESS = 5,
	PLRT_INVALID_MODE = 6,
	PLRT_ALREADY_EXISTS = 7,
	PLRT_NO_EXIST = 8,
	PLRT_IOERR = 9,

	PLRT_NOT_PLCHAR = 10,
	PLRT_NOT_COMPRESSED = 11
} plrtret_t;

void plRTErrorString(plptr_t* buffer, plrtret_t errCode);
void plRTPanic(char* msg, plrtret_t errCode, bool isDeveloperBug);
