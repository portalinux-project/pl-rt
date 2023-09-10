/*****************************************************************\
 pl-rt, v0.04
 (c) 2023 pocketlinux32, Under MPL v2.0
 plrt-error.c: Error handling module
\*****************************************************************/
#include <plrt-types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void plRTErrorString(plptr_t* buffer, plrtret_t errCode){
	if(buffer == NULL || buffer->pointer == NULL || buffer->size == 0 || (errCode & PLRT_ERROR) == 0)
		return;

	plrtret_t actualErrorCode = errCode & ~(PLRT_ERROR | PLRT_ERRNO);

	if((errCode & PLRT_ERRNO) != 0){
		char* holderPtr = strerror(actualErrorCode);
		size_t sizeOfStr = strlen(holderPtr) + 1;
		if(sizeOfStr > buffer->size)
			return;

		memcpy(buffer->pointer, holderPtr, sizeOfStr);
	}else{
		char* holderPtr = NULL;
		switch(actualErrorCode){
			case PLRT_OOM:
				holderPtr = "The system or memory tracker has run out of memory";
				break;
			case PLRT_FAILED_ALLOC:
				holderPtr = "The memory tracker failed to allocate a buffer";
				break;
			case PLRT_INVALID_PTR:
				holderPtr = "The pointer given is invalid";
				break;
			case PLRT_NULL_PTR:
				holderPtr = "The pointer(s) given is/are NULL";
				break;
			case PLRT_ACCESS:
				holderPtr = "Access to the resource has been denied";
				break;
			case PLRT_INVALID_MODE:
				holderPtr = "File mode given is invalid";
				break;
			case PLRT_ALREADY_EXISTS:
				holderPtr = "File or directory already exists";
				break;
			case PLRT_NO_EXIST:
				holderPtr = "Resource does not exist";
				break;
			case PLRT_IOERR:
				holderPtr = "Generic Input/Output Error";
				break;
			case PLRT_NOT_PLCHAR:
				holderPtr = "String is not a plChar String";
				break;
			case PLRT_NOT_COMPRESSED:
				holderPtr = "String is a plChar String";
				break;
			case PLRT_MATCH_NOT_FOUND:
				holderPtr = "Could not find a match in string";
				break;
			case PLRT_TOKENIZER_FAILURE:
				holderPtr = "Internal tokenizer failure";
				break;
			case PLRT_TOKENIZER_WTF:
				holderPtr = "what kind of fuckery where you doing mate??????";
				break;
			default:
				holderPtr = "Unknown/Application-specific Error";
				break;
		}

		if(strlen(holderPtr) + 1 > buffer->size)
			return;

		strcpy(buffer->pointer, holderPtr);
		buffer->size = strlen(holderPtr);
	}
}

void plRTPanic(char* msg, plrtret_t errCode, bool isDeveloperBug){
	char buffer[512];

	plptr_t holderStr = {
		.pointer = buffer,
		.size = 512
	};

	plRTErrorString(&holderStr, errCode);
	fprintf(stderr, "* Runtime Panic at %s: %s\n", msg, buffer);
	if(isDeveloperBug)
		fputs("* If you're a user getting this error, this is a bug. Please report to the developers or maintainers of this project\n", stderr);
	abort();
}
