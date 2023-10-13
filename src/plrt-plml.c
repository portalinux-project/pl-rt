/*******************************************\
 pl-rt, v0.05
 (c) 2022-2023 pocketlinux32, Under MPL v2.0
 plrt-plml.c: PLML parser module
\*******************************************/

plptr_t plMLSanitize(plstring_t string, plmt_t* mt){
	plptr_t retStr = plRTParse(string, mt);
	plstring_t* retStrArr = retStr.pointer;
	plchar_t sanitizerChr = { .bytes = { '#', '\0', '\0', '\0' } };

	int i = 0;
	int freeOffset = -1;
	while(i < retStr.size){
		if(freeOffset != -1){
			plMTFree(mt, retStrArr[i].data.pointer);
		}else if(plRTStrchr(retStrArr[i], sanitizerChr, 0) == 0){
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

	return retStr;
}

plmltoken_t plMLParse(plstring_t string, plmt_t* mt){
	plptr_t tokenizedStr = plMLSanitize(string, mt);
}
