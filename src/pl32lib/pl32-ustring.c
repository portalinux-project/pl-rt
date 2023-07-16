/**********************************************\
 pl32lib-ng, v1.06
 (c) 2022-2023 pocketlinux32, Under MPL v2.0
 pl32-ustring.c: UTF-8 String ops module header
\**********************************************/
#include <pl32-ustring.h>

bool isUStrNull(plstring_t* string){
	if(string == NULL || string->data.array == NULL)
		return true;

	return false;
}

size_t getCharSize(plchar_t chr){
	size_t size = 4;

	while(size > 0 && chr.bytes[size - 1] == 0)
		size--;

	return size;
}

plstring_t plUStrFromCStr(string_t cStr, plmt_t* mt){
	if(cStr == NULL)
		plPanic("plStrFromCStr: NULL pointers given!", false, true);

	size_t cStrSize = strlen(cStr);
	memptr_t tempPtr = cStr;
	bool memAlloc = false;
	if(mt != NULL){
		tempPtr = plMTAllocE(mt, cStrSize);
		memAlloc = true;
		memcpy(tempPtr, cStr, cStrSize);
	}

	plstring_t retStruct = {
		.data = {
			.array = tempPtr,
			.size = cStrSize,
			.isMemAlloc = memAlloc,
			.mt = mt
		},
		.isplChar = false
	};

	return retStruct;
}

void plUStrCompress(plstring_t* plCharStr, plmt_t* mt){
	if(mt == NULL || isUStrNull(plCharStr))
		plPanic("plStrCompress: NULL pointers given!", false, true);
	if(!plCharStr->isplChar)
		plPanic("plStrCompress: plCharStr is not a plChar string", false, true);

	plchar_t* plCharStrPtr = plCharStr->data.array;
	uint8_t* compressedStr = plMTAllocE(mt, plCharStr->data.size * 4);
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

	void* resizedPtr = plMTRealloc(mt, compressedStr, offset + 1);
	if(resizedPtr == NULL)
		plPanic("plStrCompress: Failed to reallocate memory", false, false);

	compressedStr = resizedPtr;
	if(plCharStr->data.isMemAlloc && plCharStr->data.mt != NULL)
		plMTFree(mt, plCharStr->data.array);

	plCharStr->data.array = compressedStr;
	plCharStr->data.size = offset;
	plCharStr->data.isMemAlloc = true;
	plCharStr->data.mt = mt;
}

memptr_t plMemMatch(plarray_t* memBlock1, plarray_t* memBlock2){
	if(memBlock1 == NULL || memBlock1->array == NULL || memBlock2 == NULL || memBlock2->array == NULL)
		plPanic("plMemMatch: Given memory block is NULL", false, true);

	uint8_t* mainPtr = memBlock1->array;
	uint8_t* searchPtr = memBlock2->array;
	for(int i = 0; i < memBlock1->size - memBlock2->size; i++){
		if(*(mainPtr + i) == *(searchPtr)){
			bool isThere = true;
			for(int j = 1; j < memBlock2->size; j++){
				if(*(mainPtr + i + j) != *(searchPtr + j)){
					isThere = false;
					j = memBlock2->size;
				}
			}

			if(isThere)
				return mainPtr + i;
		}
	}

	return NULL;
}

int64_t plUStrchr(plstring_t* string, plchar_t chr, size_t startAt){
	if(isUStrNull(string))
		plPanic("plUStrchr: Given string is NULL!", false, true);
	if(string->isplChar)
		plPanic("plUStrchr: Given string is a plChar array", false, true);

	plarray_t tempStruct = {
		.array = chr.bytes,
		.size = getCharSize(chr),
		.isMemAlloc = false,
		.mt = NULL
	};

	string->data.array += startAt;
	memptr_t tempPtr = plMemMatch(&string->data, &tempStruct);
	int64_t retVar = -1;
	string->data.array -= startAt;

	if(tempPtr != NULL)
		retVar = tempPtr - string->data.array;

	return retVar;
}

int64_t plUStrstr(plstring_t* string1, plstring_t* string2, size_t startAt){
	if(isUStrNull(string1) || isUStrNull(string2))
		plPanic("plUStrstr: Given string is NULL!", false, true);
	if(string1->isplChar || string2->isplChar)
		plPanic("plUStrstr: Given string is a plChar array", false, true);

	string1->data.array += startAt;
	string1->data.size -= startAt;
	memptr_t tempPtr = plMemMatch(&string1->data, &string2->data);
	int64_t retVar = -1;
	string1->data.size += startAt;
	string1->data.array -= startAt;

	if(tempPtr != NULL)
		retVar = tempPtr - string1->data.array;

	return retVar;
}

plstring_t plUStrtok(plstring_t* string, plstring_t* delimiter, plstring_t* leftoverStr, plmt_t* mt){
	if(isUStrNull(string) || isUStrNull(delimiter) || leftoverStr == NULL || mt == NULL)
		plPanic("plUStrtok: NULL was given!", false, true);
	if(string->isplChar)
		plPanic("plUStrtok: Given string is a plChar array", false, true);
	if(!delimiter->isplChar)
		plPanic("plUStrtok: Given delimiter is just a standard string", false, true);

	plstring_t retStr = {
		.data = {
			.array = NULL,
			.size = 0,
			.isMemAlloc = false,
			.mt = NULL
		},
		.isplChar = false
	};

	int iterator = 0;
	int64_t endOffset = -1;
	size_t currentPos = 0;
	size_t searchLimit = string->data.size;
	plchar_t* delim = delimiter->data.array;
	uint8_t* startPtr = string->data.array;

	while(endOffset == -1 && currentPos < searchLimit){
		size_t tempChr = plUStrchr(string, delim[iterator], currentPos);

		if(tempChr == -1){
			iterator++;
			if(iterator >= delimiter->data.size)
				endOffset = searchLimit;
		}else{
			for(int i = 0; i < delimiter->data.size; i++){
				if(i != iterator){
					size_t tempChr2 = plUStrchr(string, delim[i], currentPos);
					if(tempChr2 != -1 && tempChr2 < tempChr)
						tempChr = tempChr2;
				}
			}

			if(tempChr == 0){
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
		leftoverStr->data.array = NULL;
		leftoverStr->data.size = 0;
		return retStr;
	}

	/* Copies the memory block into the return struct */
	retStr.data.size = endOffset - currentPos + 1;
	retStr.data.array = plMTAllocE(mt, retStr.data.size);
	retStr.data.isMemAlloc = true;
	retStr.data.mt = mt;
	memcpy(retStr.data.array, string->data.array + currentPos, retStr.data.size);

	/* Calculates the pointer to put into leftoverStr*/
	leftoverStr->data.array = NULL;
	leftoverStr->data.size = 0;
	leftoverStr->isplChar = false;
	if(endOffset != searchLimit){
		size_t leftoverOffset = endOffset + getCharSize(delim[iterator]);
		iterator = 0;
		while(leftoverStr->data.array == NULL && leftoverOffset < searchLimit){
			int64_t tempChr = plUStrchr(string, delim[iterator], leftoverOffset);

			if(tempChr != 0){
				iterator++;
				if(iterator == delimiter->data.size){
					leftoverStr->data.array = startPtr + leftoverOffset;
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

plstring_t plUStrdup(plstring_t* string, bool compress, plmt_t* mt){
	if(isUStrNull(string) || mt == NULL)
		plPanic("plUStrdup: NULL was given!", false, true);

	plstring_t retStr;

	retStr.data.array = plMTAllocE(mt, string->data.size);
	retStr.data.size = string->data.size;
	memcpy(retStr.data.array, string->data.array, string->data.size);

	return retStr;
}
