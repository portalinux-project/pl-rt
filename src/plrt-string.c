/*********************************************\
 pl-rt, v1.00
 (c) 2022-2023 pocketlinux32, Under MPL v2.0
 plrt-string.c: UTF-8 String ops module header
\*********************************************/
#include <plrt-string.h>

size_t getCharSize(plchar_t chr){
	size_t size = 4;

	while(size > 0 && chr.bytes[size - 1] == 0)
		size--;

	return size;
}

plstring_t plRTStrFromCStr(char* cStr, plmt_t* mt){
	if(cStr == NULL)
		plRTPanic("plRTStrFromCStr", PLRT_ERROR | PLRT_NULL_PTR, true);

	size_t cStrSize = strlen(cStr);
	memptr_t tempPtr = cStr;
	bool memAlloc = false;
	if(mt != NULL){
		tempPtr = plMTAlloc(mt, cStrSize + 1);
		memAlloc = true;
		memcpy(tempPtr, cStr, cStrSize);
	}

	plstring_t retStruct = {
		.data = {
			.pointer = tempPtr,
			.size = cStrSize,
		},
		.mt = mt,
		.isplChar = false
	};

	return retStruct;
}

plstring_t plRTStrFromPLPtr(plptr_t pointer, plmt_t* mt, bool isplChar, bool isMemAlloc){
	plstring_t retStr = {
		.data = pointer,
		.mt = mt,
		.isplChar = isplChar
	};

	if(mt != NULL && !isMemAlloc){
		retStr.data.pointer = plMTAlloc(mt, pointer.size + 1);
		memcpy(retStr.data.pointer, pointer.pointer, pointer.size);
	}

	return retStr;
}

void plRTStrCompress(plstring_t* plCharStr, plmt_t* mt){
	if(mt == NULL || plCharStr == NULL || plCharStr->data.pointer == NULL)
		plRTPanic("plRTStrCompress", PLRT_ERROR | PLRT_NULL_PTR, true);
	if(!plCharStr->isplChar)
		plRTPanic("plRTStrCompress", PLRT_ERROR | PLRT_NOT_PLCHAR, true);

	plchar_t* plCharStrPtr = plCharStr->data.pointer;
	uint8_t* compressedStr = plMTAlloc(mt, plCharStr->data.size * 4);
	size_t offset = 0;
	for(int i = 0; i < plCharStr->data.size; i++){
		int endOfUtfChr = -1;
		uint8_t bytes[4];

		for(int j = 4; j > 0; j--){
			if(endOfUtfChr == -1 && plCharStrPtr[i].bytes[j] != 0)
				endOfUtfChr = j;
		}

		for(int j = 0; j < endOfUtfChr; j++)
			compressedStr[offset + j] = plCharStrPtr[i].bytes[j];

		offset += endOfUtfChr;
	}

	memptr_t resizedPtr = plMTRealloc(mt, compressedStr, offset + 1);
	if(resizedPtr == NULL)
		plRTPanic("plRTStrCompress", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

	compressedStr = resizedPtr;
	if(plCharStr->mt != NULL)
		plMTFree(mt, plCharStr->data.pointer);

	plCharStr->data.pointer = compressedStr;
	plCharStr->data.size = offset;
	plCharStr->mt = mt;
}

int plRTIsMemPatternDiff(uint8_t* mainPtr, plptr_t searchPtr){
	int isDiff = -1;
	uint8_t* rawSearchPtr = searchPtr.pointer;

	if(*mainPtr == *rawSearchPtr){
		for(int j = 1; j < searchPtr.size; j++){
			if(*(mainPtr + j) != *(rawSearchPtr + j)){
				isDiff = j;
				j = searchPtr.size;
			}
		}

		return isDiff;
	}

	return 0;
}

memptr_t plRTMemMatch(plptr_t memBlock1, plptr_t memBlock2){
	if(memBlock1.pointer == NULL || memBlock2.pointer == NULL)
		plRTPanic("plRTMemMatch", PLRT_ERROR | PLRT_NULL_PTR, true);

	for(int i = 0; i <= memBlock1.size - memBlock2.size; i++){
		if(plRTIsMemPatternDiff(memBlock1.pointer + i, memBlock2) == -1)
			return memBlock1.pointer + i;
	}

	return NULL;
}

int64_t plRTStrchr(plstring_t string, plchar_t chr, size_t startAt){
	if(string.data.pointer == NULL)
		plRTPanic("plRTStrchr", PLRT_ERROR | PLRT_NULL_PTR, true);
	if(string.isplChar)
		plRTPanic("plRTStrchr", PLRT_ERROR | PLRT_NOT_COMPRESSED, true);

	plptr_t tempStruct = {
		.pointer = chr.bytes,
		.size = getCharSize(chr),
	};

	string.data.pointer += startAt;
	memptr_t tempPtr = plRTMemMatch(string.data, tempStruct);
	int64_t retVar = -1;
	string.data.pointer -= startAt;

	if(tempPtr != NULL)
		retVar = tempPtr - string.data.pointer;

	return retVar;
}

int64_t plRTStrstr(plstring_t string1, plstring_t string2, size_t startAt){
	if(string1.data.pointer == NULL || string2.data.pointer == NULL)
		plRTPanic("plRTStrstr", PLRT_ERROR | PLRT_NULL_PTR, true);
	if(string1.isplChar || string2.isplChar)
		plRTPanic("plRTStrstr", PLRT_ERROR | PLRT_NOT_COMPRESSED, true);

	string1.data.pointer += startAt;
	string1.data.size -= startAt;
	memptr_t tempPtr = plRTMemMatch(string1.data, string2.data);
	int64_t retVar = -1;
	string1.data.pointer -= startAt;

	if(tempPtr != NULL)
		retVar = tempPtr - string1.data.pointer;

	return retVar;
}

int plRTStrcmp(plstring_t string1, plstring_t string2){
	if(string1.data.size > string2.data.size)
		return ((uint8_t*)string1.data.pointer)[string2.data.size - 1];
	else if(string1.data.size < string2.data.size)
		return ((uint8_t*)string2.data.pointer)[string1.data.size - 1];

	int diffChar = plRTIsMemPatternDiff(string1.data.pointer, string2.data);
	if(diffChar != 1){
		return ((uint8_t*)string1.data.pointer)[diffChar] - ((uint8_t*)string2.data.pointer)[diffChar];
	}

	return 0;
}

plstring_t plRTStrtok(plstring_t string, plstring_t delimiter, plstring_t* leftoverStr, plmt_t* mt){
	if(delimiter.data.pointer == NULL || leftoverStr == NULL || mt == NULL)
		plRTPanic("plRTStrtok", PLRT_ERROR | PLRT_NULL_PTR, true);
	if(string.isplChar)
		plRTPanic("plRTStrtok", PLRT_ERROR | PLRT_NOT_COMPRESSED, true);
	if(!delimiter.isplChar)
		plRTPanic("plRTStrtok", PLRT_ERROR | PLRT_NOT_PLCHAR, true);

	plstring_t retStr = {
		.data = {
			.pointer = NULL,
			.size = 0,
		},
		.mt = NULL,
		.isplChar = false
	};

	if(string.data.pointer == NULL)
		return retStr;

	int iterator = 0;
	int64_t endOffset = -1;
	int64_t currentPos = 0;
	size_t searchLimit = string.data.size;
	plchar_t* delim = delimiter.data.pointer;
	uint8_t* startPtr = string.data.pointer;

	while(endOffset == -1 && currentPos < searchLimit){
		int64_t tempChr = plRTStrchr(string, delim[iterator], currentPos);

		if(tempChr == -1){
			iterator++;
			if(iterator >= delimiter.data.size)
				endOffset = searchLimit;
		}else{
			for(int i = 0; i < delimiter.data.size; i++){
				if(i != iterator){
					int64_t tempChr2 = plRTStrchr(string, delim[i], currentPos);
					if(tempChr2 != -1 && tempChr2 < tempChr)
						tempChr = tempChr2;
				}
			}

			if(tempChr - currentPos == 0){
				if(startPtr[currentPos] >= 128){
					while(startPtr[currentPos] >= 128)
						currentPos++;
				}else{
					currentPos++;
				}

				iterator = 0;
			}else{
				endOffset = tempChr;
			}
		}

	}

	if(endOffset == -1){
		leftoverStr->data.pointer = NULL;
		leftoverStr->data.size = 0;
		return retStr;
	}

	/* Copies the memory block into the return struct */
	retStr.data.size = endOffset - currentPos;
	retStr.data.pointer = plMTAlloc(mt, retStr.data.size + 1);
	retStr.mt = mt;
	memcpy(retStr.data.pointer, string.data.pointer + currentPos, retStr.data.size);
	((char*)retStr.data.pointer)[retStr.data.size] = '\0';

	/* Calculates the pointer to put into leftoverStr*/
	leftoverStr->data.pointer = NULL;
	leftoverStr->data.size = 0;
	leftoverStr->isplChar = false;
	if(endOffset != searchLimit){
		size_t leftoverOffset = endOffset + getCharSize(delim[iterator]);
		iterator = 0;
		while(leftoverStr->data.pointer == NULL && leftoverOffset < searchLimit){
			int64_t tempChr = plRTStrchr(string, delim[iterator], leftoverOffset);

			if(tempChr != 0){
				iterator++;
				if(iterator == delimiter.data.size){
					leftoverStr->data.pointer = startPtr + leftoverOffset;
					leftoverStr->data.size = searchLimit - leftoverOffset;
					leftoverOffset = searchLimit;
				}
			}else{
				leftoverOffset += getCharSize(delim[iterator]);
			}
		}
	}


	return retStr;
}

plstring_t plRTStrdup(plstring_t string, bool compress, plmt_t* mt){
	if(string.data.pointer || mt == NULL)
		plRTPanic("plRTStrdup", PLRT_ERROR | PLRT_NULL_PTR, true);

	plstring_t retStr;

	retStr.data.pointer = plMTAlloc(mt, string.data.size + 1);
	retStr.data.size = string.data.size;
	retStr.isplChar = false;
	retStr.mt = mt;
	memcpy(retStr.data.pointer, string.data.pointer, string.data.size);

	return retStr;
}
