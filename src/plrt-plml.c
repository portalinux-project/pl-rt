/*******************************************\
 pl-rt, v0.05
 (c) 2022-2023 pocketlinux32, Under MPL v2.0
 plrt-plml.c: PLML parser module
\*******************************************/
#include <plrt-plml.h>

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
				tempPtr = plMTRealloc(mt, retStrArr[useOffset].data.pointer, retStrArr[useOffset].data.size - 1);
				if(tempPtr == NULL)
					plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

				((char*)tempPtr)[retStrArr[useOffset].data.size - 2] = '\0';
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
					retToken.type = PLML_TYPE_STRING;
					retToken.value.string = tokenStr.data;
				}
			}else{
				plMTFree(mt, rawTokenStr);
			}
		}else{
			plMTFree(mt, rawTokenStr);
		}

		plMTFree(mt, tokenizedStr.pointer);
	}

	return retToken;
}

void plMLFreeToken(plmltoken_t token){
	plMTFree((plmt_t*)token.mt, token.name.data.pointer);
	if(token.type == PLML_TYPE_STRING)
		plMTFree((plmt_t*)token.mt, token.value.string.pointer);
}
