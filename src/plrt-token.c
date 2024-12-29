/*******************************************\
 pl-rt, v1.04
 (c) 2024 CinnamonWolfy, Under MPL v2.0
 plrt-token.c: Parser/Tokenizer module
\*******************************************/
#include <plrt-token.h>

plstring_t plRTTokenize(plstring_t string, plstring_t* leftoverStr, plmt_t* mt){
	if(leftoverStr == NULL || mt == NULL)
		plRTPanic("plRTTokenize", PLRT_ERROR | PLRT_NULL_PTR, true);

	plchar_t delimiters[8] = {
		{
			.bytes = { '"', '\0', '\0', '\0' }
		},
		{
			.bytes = { '\'', '\0', '\0', '\0' }
		},
		{
			.bytes = { '[', '\0', '\0', '\0' }
		},
		{
			.bytes = { ']', '\0', '\0', '\0' }
		},
		{
			.bytes = { ' ', '\0', '\0', '\0' }
		},
		{
			.bytes = { '\n', '\0', '\0', '\0' }
		},
		{
			.bytes = { '\t', '\0', '\0', '\0' }
		},
		{
			.bytes = { '\\', '\0', '\0', '\0' }
		},
	};

	plstring_t retStr = {
		.data = {
			.pointer = NULL,
			.size = 0
		},
		.mt = mt,
		.isplChar = false
	};

	if(string.data.size == 0)
		return retStr;

	memptr_t tempSearchLimit = string.data.pointer + string.data.size - 1;
	while(string.data.pointer < tempSearchLimit && (*(char*)string.data.pointer == ' ' || *(char*)string.data.pointer == '\t' || *(char*)string.data.pointer == '\n')){
		string.data.pointer++;
		string.data.size--;
	}

	if(string.data.pointer == tempSearchLimit && (*(char*)string.data.pointer == ' ' || *(char*)string.data.pointer == '\t' || *(char*)string.data.pointer == '\n')){
		leftoverStr->data.pointer = NULL;
		leftoverStr->data.size = 0;
		return retStr;
	}

	int64_t delimOffsets[6] = { plRTStrchr(string, delimiters[0], 0), plRTStrchr(string, delimiters[1], 0), plRTStrchr(string, delimiters[2], 0), -1, plRTStrchr(string, delimiters[4], 0), plRTStrchr(string, delimiters[6], 0)};
	int64_t quoteEndings[2] = { -1, -1 };

	bool noQuotesFound = (delimOffsets[0] == -1 && delimOffsets[1] == -1);
	bool noEndQuoteBasic = (delimOffsets[0] != -1 && (quoteEndings[0] = plRTStrchr(string, delimiters[0], delimOffsets[0] + 1)) == -1);
	bool noEndQuoteLiteral = (delimOffsets[1] != -1 && (quoteEndings[1] = plRTStrchr(string, delimiters[1], delimOffsets[1] + 1)) == -1);
	bool noEndArrayBracket = (delimOffsets[2] != -1 && (delimOffsets[3] = plRTStrchr(string, delimiters[3], delimOffsets[2] + 1)) == -1);
	bool spaceComesFirst = (delimOffsets[4] != -1 && (delimOffsets[0] == -1 || delimOffsets[4] < delimOffsets[0]) && (delimOffsets[1] == -1 || delimOffsets[4] < delimOffsets[1]) && (delimOffsets[2] == -1 || delimOffsets[4] < delimOffsets[2]));
	bool arrayBeforeStr = (delimOffsets[2] != -1 && (delimOffsets[0] == -1 || delimOffsets[2] < delimOffsets[0]) && (delimOffsets[1] == -1 || delimOffsets[2] < delimOffsets[1]));
	bool literalBeforeBasicStr = (delimOffsets[1] != -1 && (delimOffsets[0] == -1 || delimOffsets[1] < delimOffsets[0]));
	bool arrayIsNotFirstChar = (delimOffsets[2] > 0);
	bool basicQuoteIsNotFirstChar = (delimOffsets[0] > 0);
	bool literalQuoteIsNotFirstChar = (delimOffsets[1] > 0);

	if((noQuotesFound && delimOffsets[2] == -1) || (noEndArrayBracket && arrayBeforeStr) || (noEndQuoteBasic && !literalBeforeBasicStr) || (noEndQuoteLiteral && literalBeforeBasicStr) || spaceComesFirst){
		retStr.data.pointer = &delimiters[4];
		retStr.data.size = 3;
		retStr.isplChar = true;

		return plRTStrtok(string, retStr, leftoverStr, mt);
	}else{
		int64_t searchLimit = string.data.size;
		int64_t startOffset = 0;
		int64_t endOffset = 0;

		if(arrayBeforeStr && !arrayIsNotFirstChar && !noEndArrayBracket){
			endOffset = delimOffsets[3];
		}else if(literalBeforeBasicStr && !literalQuoteIsNotFirstChar && !noEndQuoteLiteral){
			endOffset = quoteEndings[1];
		}else if((arrayIsNotFirstChar && arrayBeforeStr) || (literalBeforeBasicStr && literalQuoteIsNotFirstChar) || basicQuoteIsNotFirstChar){
			startOffset = 0;
			if(arrayBeforeStr)
				endOffset = delimOffsets[2] - 1;
			else if(!literalBeforeBasicStr)
				endOffset = delimOffsets[0] - 1;
			else
				endOffset = delimOffsets[1] - 1;
		}else{
			endOffset = quoteEndings[0];
			while(endOffset != -1 && ((char*)string.data.pointer)[endOffset - 1] == '\\')
				endOffset = plRTStrchr(string, delimiters[0], endOffset + 1);

			if(endOffset == -1)
				plRTPanic("plRTTokenize", PLRT_ERROR | PLRT_TOKENIZER_WTF, true);
		}

		retStr.data.size = endOffset + 1;
		retStr.data.pointer = plMTAlloc(mt, retStr.data.size + 1);
		memcpy(retStr.data.pointer, string.data.pointer + startOffset, retStr.data.size);
		((char*)retStr.data.pointer)[retStr.data.size] = '\0';

		int64_t escapedChars = plRTStrchr(retStr, delimiters[7], 0);
		if(escapedChars != -1 && *((char*)retStr.data.pointer) == '"'){
			while(escapedChars != -1){
				memcpy(retStr.data.pointer + escapedChars, retStr.data.pointer + escapedChars + 1, retStr.data.size - escapedChars);
				retStr.data.size--;
				escapedChars++;
				escapedChars = plRTStrchr(retStr, delimiters[7], escapedChars);
			}

			memptr_t tempPtr = plMTRealloc(mt, retStr.data.pointer, retStr.data.size);
			if(tempPtr == NULL)
				plRTPanic("plRTTokenize", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

			retStr.data.pointer = tempPtr;
		}

		leftoverStr->isplChar = false;
		if(endOffset == searchLimit || (endOffset == searchLimit - 1 && (((char*)retStr.data.pointer)[searchLimit] == ' ' || ((char*)retStr.data.pointer)[searchLimit] == '\n' || ((char*)retStr.data.pointer)[searchLimit] == '\t'))){
			leftoverStr->data.pointer = NULL;
			leftoverStr->data.size = 0;
		}else{
			if((((char*)string.data.pointer)[endOffset] == '"' && basicQuoteIsNotFirstChar) || (((char*)string.data.pointer)[endOffset] == '\'' && literalQuoteIsNotFirstChar) || (((char*)string.data.pointer)[endOffset] == '[' && arrayIsNotFirstChar)){
				leftoverStr->data.pointer = string.data.pointer + endOffset;
				leftoverStr->data.size = string.data.size - endOffset;
			}else{
				leftoverStr->data.pointer = string.data.pointer + endOffset + 1;
				leftoverStr->data.size = string.data.size - (endOffset + 1);
			}
		}

		return retStr;
	}
}

plptr_t plRTParser(plstring_t string, plmt_t* mt){
	if(string.data.pointer == NULL || mt == NULL)
		plRTPanic("plRTParse", PLRT_ERROR | PLRT_NULL_PTR, true);

	plptr_t retPtr = {
		.pointer = plMTAlloc(mt, 2 * sizeof(plstring_t)),
		.size = 0
	};

	plstring_t leftoverStr;
	plstring_t holderStr = plRTTokenize(string, &leftoverStr, mt);
	((plstring_t*)retPtr.pointer)[retPtr.size] = holderStr;
	retPtr.size++;

	while(leftoverStr.data.pointer != NULL && *((char*)leftoverStr.data.pointer) != '\0'){
		if(retPtr.size > 1){
			memptr_t tempPtr = plMTRealloc(mt, retPtr.pointer, (retPtr.size + 1) * sizeof(plstring_t));
			if(tempPtr == NULL)
				plRTPanic("plRTParse", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

			retPtr.pointer = tempPtr;
		}
		holderStr = plRTTokenize(leftoverStr, &leftoverStr, mt);
		if(holderStr.data.pointer != NULL){
			((plstring_t*)retPtr.pointer)[retPtr.size] = holderStr;
			retPtr.size++;
		}
	}

	return retPtr;
}

void plRTFreeParsedString(plptr_t stringArray){
	if(stringArray.pointer == NULL)
		plRTPanic("plRTFreeParsedString", PLRT_ERROR | PLRT_NULL_PTR, false);

	plstring_t* stringArrPtr = stringArray.pointer;
	plmt_t* mt = stringArrPtr[0].mt;

	for(int i = 0; i < stringArray.size; i++){
		plMTFree(mt, stringArrPtr[0].data.pointer);
	}
	plMTFree(mt, stringArrPtr);
}

void plRTExtractContents(plstring_t* string){
	if(*((char*)string->data.pointer) == '"' || *((char*)string->data.pointer) == '\'' || *((char*)string->data.pointer) == '['){
		memcpy(string->data.pointer, string->data.pointer + 1, string->data.size - 2);
		memptr_t tempPtr = plMTRealloc(string->mt, string->data.pointer, string->data.size - 2);
		if(tempPtr == NULL)
			plRTPanic("plRTExtractContents", PLRT_ERROR | PLRT_NULL_PTR, false);
		((char*)tempPtr)[string->data.size - 2] = '\0';
		string->data.pointer = tempPtr;
		string->data.size -= 2;
	}
}
