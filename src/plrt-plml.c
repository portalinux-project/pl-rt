/*******************************************\
 pl-rt, v1.00
 (c) 2022-2023 pocketlinux32, Under MPL v2.0
 plrt-plml.c: PLML parser module
\*******************************************/
#include <plrt-plml.h>

plsimpletoken_t plMLGetValue(plstring_t tokenStr, plmt_t* mt){
	plsimpletoken_t retToken;
	char* rawTokenStr = tokenStr.data.pointer;
	char* leftoverStr = NULL;

	retToken.type = PLML_TYPE_INT;
	retToken.value.integer = strtol(rawTokenStr, &leftoverStr, 10);
	if(leftoverStr != NULL && *leftoverStr != '\0'){
		retToken.type = PLML_TYPE_FLOAT;
		retToken.value.decimal = strtod(rawTokenStr, &leftoverStr);
		if(leftoverStr != NULL && *leftoverStr != '\0'){
			retToken.type = PLML_TYPE_BOOL;
			if(strcmp(rawTokenStr, "true") == 0){
				retToken.value.boolean = true;
				plMTFree(mt, rawTokenStr);
			}else if(strcmp(rawTokenStr, "false") == 0){
				retToken.value.boolean = false;
				plMTFree(mt, rawTokenStr);
			}else{
				plRTExtractContents(&tokenStr);
				retToken.type = PLML_TYPE_STRING;
				retToken.value.string = tokenStr.data;
			}
		}else{
			plMTFree(mt, rawTokenStr);
		}
	}else{
		plMTFree(mt, rawTokenStr);
	}

	return retToken;
}

plsimpletoken_t plMLParseArray(plstring_t tokenName, plstring_t arrString, plmt_t* mt){
	plsimpletoken_t retToken;

	plRTExtractContents(&arrString);
	char* rawArrString = arrString.data.pointer;
	plchar_t commaPLChar = {
		.bytes = { ',', '\0', '\0', '\0' }
	};

	int64_t offsetHolder[2] = { -1, -1 };
	while((offsetHolder[0] = plRTStrchr(arrString, commaPLChar, offsetHolder[1] + 1)) != -1){
		rawArrString[offsetHolder[0]] = ' ';
		offsetHolder[1] = offsetHolder[0];
	}

	plptr_t parsedArr = plRTParser(arrString, mt);
	plsimpletoken_t arrMember = plMLGetValue(((plstring_t*)parsedArr.pointer)[0], mt);
	retToken.type = arrMember.type;

	size_t memberSize = 0;
	switch(retToken.type){
		case PLML_TYPE_INT: ;
			memberSize = sizeof(long);
			retToken.value.array.pointer = plMTAlloc(mt, 2 * memberSize);
			((long*)retToken.value.array.pointer)[0] = arrMember.value.integer;
			break;
		case PLML_TYPE_BOOL: ;
			memberSize = sizeof(bool);
			retToken.value.array.pointer = plMTAlloc(mt, 2 * memberSize);
			((bool*)retToken.value.array.pointer)[0] = arrMember.value.boolean;
			break;
		case PLML_TYPE_FLOAT: ;
			memberSize = sizeof(double);
			retToken.value.array.pointer = plMTAlloc(mt, 2 * memberSize);
			((double*)retToken.value.array.pointer)[0] = arrMember.value.decimal;
			break;
		case PLML_TYPE_STRING: ;
			memberSize = sizeof(plptr_t);
			retToken.value.array.pointer = plMTAlloc(mt, 2 * memberSize);
			((plptr_t*)retToken.value.array.pointer)[0] = arrMember.value.string;
			break;
		default: ;
	}
	retToken.value.array.size = 1;

	for(int i = 1; i < parsedArr.size; i++){
		if(retToken.value.array.size > 1){
			memptr_t tempPtr = plMTRealloc(mt, retToken.value.array.pointer, (retToken.value.array.size + 1) * memberSize);
			if(tempPtr == NULL)
				plRTPanic("plMLParseArray", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

			retToken.value.array.pointer = tempPtr;
		}

		arrMember = plMLGetValue(((plstring_t*)parsedArr.pointer)[i], mt);
		if(arrMember.type != retToken.type)
			plRTPanic("plMLParseArray", PLRT_ERROR | PLRT_INVALID_TOKEN, false);

		switch(retToken.type){
			case PLML_TYPE_INT:
				((int*)retToken.value.array.pointer)[i] = arrMember.value.integer;
				break;
			case PLML_TYPE_BOOL:
				((bool*)retToken.value.array.pointer)[i] = arrMember.value.boolean;
				break;
			case PLML_TYPE_FLOAT:
				((double*)retToken.value.array.pointer)[i] = arrMember.value.decimal;
				break;
			case PLML_TYPE_STRING:
				((plptr_t*)retToken.value.array.pointer)[i] = arrMember.value.string;
				break;
			default: ;
		}

		retToken.value.array.size++;

	}

	plMTFree(mt, arrString.data.pointer - 1);
	return retToken;
}

plptr_t plMLSanitize(plstring_t string, plmt_t* mt){
	plptr_t retStr = plRTParser(string, mt);
	plstring_t* retStrArr = retStr.pointer;
	plchar_t sanitizerChrs[2] = { { .bytes = { '#', '\0', '\0', '\0' } }, { .bytes = { '=', '\0', '\0', '\0' } } };

	int i = 0;
	int freeOffset = -1;
	while(i < retStr.size){
		if(freeOffset != -1){
			plMTFree(mt, retStrArr[i].data.pointer);
		}else if(plRTStrchr(retStrArr[i], sanitizerChrs[0], 0) == 0){
			freeOffset = i;
			plMTFree(mt, retStrArr[i].data.pointer);
		}
		i++;
	}

	if(freeOffset != -1){
		retStr.size = freeOffset;
		memptr_t tempPtr = plMTRealloc(mt, retStr.pointer, retStr.size * sizeof(plstring_t));
		if(tempPtr == NULL)
			plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

		retStr.pointer = tempPtr;
	}

	if(retStr.size > 1){
		bool stillInvalid = true;

		if(retStr.size == 2){
			bool useOffset = 0;
			int64_t offsets[2] = { plRTStrchr(retStrArr[0], sanitizerChrs[1], 0), plRTStrchr(retStrArr[1], sanitizerChrs[1], 0) };

			if(offsets[0] == retStrArr[0].data.size - 1 || offsets[1] == 0){
				stillInvalid = false;
				if(offsets[1] == 0)
					useOffset = 1;

				memptr_t tempPtr = NULL;
				if(useOffset == 1)
					memcpy(retStrArr[useOffset].data.pointer, retStrArr[useOffset].data.pointer + 1, retStrArr[useOffset].data.size - 1);

				((char*)retStrArr[useOffset].data.pointer)[retStrArr[useOffset].data.size - 1] = '\0';
				tempPtr = plMTRealloc(mt, retStrArr[useOffset].data.pointer, retStrArr[useOffset].data.size);
				if(tempPtr == NULL)
					plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

				retStrArr[useOffset].data.pointer = tempPtr;
				retStrArr[useOffset].data.size--;
			}

			stillInvalid = false;
		}else if(retStr.size == 3){
			plMTFree(mt, retStrArr[1].data.pointer);
			retStrArr[1] = retStrArr[2];
			memptr_t tempPtr = plMTRealloc(mt, retStr.pointer, (retStr.size - 1) * sizeof(plstring_t));
			if(tempPtr == NULL)
				plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

			retStr.pointer = tempPtr;
			retStr.size--;

			stillInvalid = false;
		}

		if(stillInvalid)
			plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_INVALID_TOKEN, false);
	}else if(retStr.size == 1){
		int64_t equalSign = plRTStrchr(retStrArr[0], sanitizerChrs[1], 0);
		char* actualString = retStrArr[0].data.pointer;

		if((actualString[0] == '[' && equalSign != -1) || (actualString[0] != '[' && equalSign == -1))
			plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_INVALID_TOKEN, false);

		if(actualString[0] != '['){
			actualString[equalSign] = ' ';
			plptr_t newRetStr = plRTParser(retStrArr[0], mt);
			plMTFree(mt, actualString);
			plMTFree(mt, retStr.pointer);
			retStr = newRetStr;
		}
	}else{
		plMTFree(mt, retStr.pointer);
		retStr.pointer = NULL;
		retStr.size = 0;
	}

	return retStr;
}

plmltoken_t plMLParse(plstring_t string, plmt_t* mt){
	plptr_t tokenizedStr = plMLSanitize(string, mt);
	plmltoken_t retToken = {
		.name = {
			.data = {
				.pointer = NULL,
				.size = 0
			},
			.mt = NULL,
			.isplChar = false
		},
		.isArray = false,
		.type = PLML_TYPE_NULL,
		.mt = mt
	};

	if(tokenizedStr.size == 0)
		return retToken;

	if(tokenizedStr.size == 1){
		plstring_t* nameStr = (plstring_t*)tokenizedStr.pointer;
		char* stringPtr = nameStr->data.pointer;
		size_t stringSize = nameStr->data.size;

		memcpy(stringPtr, stringPtr + 1, stringSize - 1);
		void* tempPtr = plMTRealloc(mt, stringPtr, stringSize - 1);
		stringPtr[stringSize - 2] = '\0';
		if(tempPtr == NULL)
			plRTPanic("plMLParse", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

		nameStr->data.pointer = stringPtr;
		nameStr->data.size = stringSize - 2;
		retToken.name = *nameStr;
		retToken.type = PLML_TYPE_HEADER;
	}else{
		retToken.name = ((plstring_t*)tokenizedStr.pointer)[0];
		plstring_t tokenStr = ((plstring_t*)tokenizedStr.pointer)[1];
		plsimpletoken_t simpleToken;
		if(*((char*)tokenStr.data.pointer) == '['){
			retToken.isArray = true;
			simpleToken = plMLParseArray(retToken.name, tokenStr, mt);
		}else{
			simpleToken = plMLGetValue(tokenStr, mt);
		}
		retToken.type = simpleToken.type;
		retToken.value = simpleToken.value;

		plMTFree(mt, tokenizedStr.pointer);
	}

	return retToken;
}

void plMLFreeToken(plmltoken_t token){
	plMTFree((plmt_t*)token.mt, token.name.data.pointer);
	if(token.isArray){
		if(token.type == PLML_TYPE_STRING){
			for(int i = 0; i < token.value.array.size; i++)
				plMTFree((plmt_t*)token.mt, ((plptr_t*)token.value.array.pointer)[i].pointer);
		}

		plMTFree((plmt_t*)token.mt, token.value.array.pointer);
	}else if(token.type == PLML_TYPE_STRING){
		plMTFree((plmt_t*)token.mt, token.value.string.pointer);
	}
}
