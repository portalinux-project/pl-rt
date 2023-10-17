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
	}

	if(freeOffset != -1){
		retStr.size = freeOffset + 1;
		memptr_t tempPtr = plMTRealloc(mt, retStr.pointer, retStr.size * sizeof(plstring_t));
		if(tempPtr == NULL)
			plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

		retStr.pointer = tempPtr;
	}

	if(retStr.size > 3 && retStr.size != 1){
		bool stillInvalid = true;
		if(retStr.size == 2){
			bool useOffset = 0;
			int64_t offsets[2] = { plRTStrchr(retStrArr[0], sanitizerChrs[1], 0), plRTStrchr(retStrArr[1], sanitizerChrs[1], 0) };

			if(offsets[0] == retStrArr[0].data.size - 1 || offsets[1] == 0){
				stillInvalid = false;
				if(offsets[1] == 0)
					useOffset = 1;

				memptr_t tempPtr = NULL;
				memcpy(retStrArr[useOffset].data.pointer, retStrArr[useOffset].data.pointer + 1, retStrArr[useOffset].data.size - 1);
				tempPtr = plMTRealloc(mt, retStrArr[useOffset].data.pointer, retStrArr[useOffset].data.size - 1);
				if(tempPtr == NULL)
					plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

				retStrArr[useOffset].data.pointer = tempPtr;
				retStrArr[useOffset].data.size--;
			}
		}else if(retStr.size == 3){
			plMTFree(mt, retStrArr[1].data.pointer);
			retStrArr[1] = retStrArr[2];
			memptr_t tempPtr = plMTRealloc(mt, retStr.pointer, retStr.size - 1);
			if(tempPtr == NULL)
				plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_FAILED_ALLOC, false);

			retStr.pointer = tempPtr;
			retStr.size--;
		}

		if(stillInvalid)
			plRTPanic("plMLSanitize", PLRT_ERROR | PLRT_INVALID_TOKEN, false);
	}

	return retStr;
}

plmltoken_t plMLParse(plstring_t string, plmt_t* mt){
	plptr_t tokenizedStr = plMLSanitize(string, mt);

	if(tokenizedStr.size == 1){

	}else{

	}
}
