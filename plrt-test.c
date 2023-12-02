/****************************************\
* plrt-test: pl-rt testcase              *
* (c)2023 pocketlinux32, Under MPL v2.0  *
\****************************************/
#include <plrt.h>

bool nonInteractive = false;

void printArray(int* array, size_t size){
	printf("Printing out array:\n");
	for(int i = 0; i < size; i++){
		printf("	array[%d]: %d\n", i, array[i]);
	}
}

void printCurrentMemUsg(plmt_t* mt){
	byte_t gchVar;

	printf("Current RAM usage: %ld bytes\n", plMTMemAmnt(mt, PLMT_GET_USEDMEM, 0));
	printf("Press Enter to continue test...");
	fflush(stdout);
	if(!nonInteractive)
		gchVar = getchar();
	else
		fputs("\n", stdout);
}

int plRTMemoryTest(plmt_t* mt){
	printCurrentMemUsg(mt);

	printf("Allocating and initializing int array (4 ints)...");

	int* nano = plMTAlloc(mt, sizeof(int) * 4);
	for(int i = 0; i < 4; i++){
		nano[i] = i + 1;
	}

	printf("Done.\n");

	printArray(nano, 4);
	printCurrentMemUsg(mt);

	printf("Reallocating int array...");

	memptr_t tempPtr = plMTRealloc(mt, nano, sizeof(int) * 8);
	nano = tempPtr;
	printf("Done\n");
	printCurrentMemUsg(mt);

	printf("Deallocating int array...");
	plMTFree(mt, nano);
	printf("Done\n");
	printCurrentMemUsg(mt);

	printf("Testing double free protection...");
	plMTFree(mt, nano);
	printf("Done\n");

	printf("Allocating multiple arrays of multiple sizes...");

	char* nano2 = plMTAlloc(mt, sizeof(byte_t) * 16);
	char** nano3 = plMTAlloc(mt, sizeof(char*) * 4);
	int* nano4 = plMTAlloc(mt, sizeof(int) * 10);
	int* nano5 = plMTAlloc(mt, sizeof(int) * 20);

	printf("Done\n");
	printCurrentMemUsg(mt);

	printf("Deallocating arrays...");

	plMTFree(mt, nano2);
	plMTFree(mt, nano3);
	plMTFree(mt, nano4);
	plMTFree(mt, nano5);

	printf("Done\n");
	printCurrentMemUsg(mt);

	return 0;
}

int plRTStringTest(plmt_t* mt){
	plstring_t convertedStr = plRTStrFromCStr("hewwo wowwd :3", mt);
	plstring_t plCharString = {
		.data = {
			.pointer = plMTAlloc(mt, 14 * sizeof(plchar_t)),
			.size = 14,
		},
		.mt = mt,
		.isplChar = true
	};

	((plchar_t*)plCharString.data.pointer)[0].bytes[0] = 'h';
	((plchar_t*)plCharString.data.pointer)[1].bytes[0] = 'e';
	((plchar_t*)plCharString.data.pointer)[2].bytes[0] = 'w';
	((plchar_t*)plCharString.data.pointer)[3].bytes[0] = 'w';
	((plchar_t*)plCharString.data.pointer)[4].bytes[0] = 'o';
	((plchar_t*)plCharString.data.pointer)[5].bytes[0] = ' ';
	((plchar_t*)plCharString.data.pointer)[6].bytes[0] = 'w';
	((plchar_t*)plCharString.data.pointer)[7].bytes[0] = 'o';
	((plchar_t*)plCharString.data.pointer)[8].bytes[0] = 'w';
	((plchar_t*)plCharString.data.pointer)[9].bytes[0] = 'w';
	((plchar_t*)plCharString.data.pointer)[10].bytes[0] = 'd';
	((plchar_t*)plCharString.data.pointer)[11].bytes[0] = ' ';
	((plchar_t*)plCharString.data.pointer)[12].bytes[0] = ':';
	((plchar_t*)plCharString.data.pointer)[13].bytes[0] = '3';
	plRTStrCompress(&plCharString, mt);

	fputs("Converted C String: ", stdout);
	fwrite(convertedStr.data.pointer, 1, convertedStr.data.size, stdout);
	fputs("\n", stdout);

	fputs("Compressed plchar_t string: ", stdout);
	fwrite(plCharString.data.pointer, 1, plCharString.data.size, stdout);
	fputs("\n\n", stdout);

	plstring_t matchStr = plRTStrFromCStr("wo", NULL);
	plchar_t plChr = {
		.bytes = { 'w', '\0', '\0', '\0'  }
	};

	puts("plchar_t-based Matching Test");

	int64_t retIndex = plRTStrchr(convertedStr, plChr, 0);
	if(retIndex == -1)
		plRTPanic("plRTStringTest", PLRT_ERROR | PLRT_MATCH_NOT_FOUND, true);

	printf("Returned Offset: %ld\n", retIndex);
	printf("Current Value: %c\n\n", *((char*)convertedStr.data.pointer + retIndex));

	puts("plstring_t-based Matching Test");

	retIndex = plRTStrstr(convertedStr, matchStr, 5);
	if(retIndex == -1)
		plRTPanic("plRTStringTest", PLRT_ERROR | PLRT_MATCH_NOT_FOUND, true);

	printf("Returned Offset: %ld\n", retIndex);
	fputs("Current Value: ", stdout);
	fwrite(convertedStr.data.pointer + retIndex, 1, convertedStr.data.size - retIndex, stdout);
	fputs("\n\n", stdout);

	plChr.bytes[0] = ' ';
	plstring_t holderStr = {
		.data = {
			.pointer = NULL,
			.size = 0,
		},
		.mt = NULL,
		.isplChar = false
	};
	plstring_t leftoverStr;
	plstring_t delimiterArr = {
		.data = {
			.pointer = &plChr,
			.size = 1,
		},
		.mt = NULL,
		.isplChar = true
	};
	plstring_t tokenizedStr = plRTStrtok(convertedStr, delimiterArr, &holderStr, mt);
	memcpy(&leftoverStr, &holderStr, sizeof(plstring_t));

	puts("plstring_t-based String Tokenizer Test");

	if(tokenizedStr.data.pointer == NULL)
		plRTPanic("plRTStringTest", PLRT_ERROR | PLRT_TOKENIZER_FAILURE, true);

	fputs("Current Value: ", stdout);
	fwrite(tokenizedStr.data.pointer, 1, tokenizedStr.data.size, stdout);
	fputs("\n", stdout);

	for(int i = 0; i < 2; i++){
		tokenizedStr = plRTStrtok(leftoverStr, delimiterArr, &holderStr, mt);
		memcpy(&leftoverStr, &holderStr, sizeof(plstring_t));
		fputs("Current Value: ", stdout);
		fwrite(tokenizedStr.data.pointer, 1, tokenizedStr.data.size, stdout);
		fputs("\n", stdout);
	}

	return 0;
}

int plRTFileTest(char* customFile, plmt_t* mt){
	plstring_t stringBuffer = {
		.data = {
			.pointer = plMTAlloc(mt, 4096),
			.size = 4096
		},
		.mt = mt,
		.isplChar = false
	};

	char filepath[256] = "src/plrt-file.c";
	if(customFile != NULL)
		strncpy(filepath, customFile, 256);

	printf("Opening an existing file...");
	plfile_t* realFile = plFOpen(filepath, "r", mt);
	plfile_t* memFile = plFOpen(NULL, "w+", mt);

	if(realFile == NULL){
		printf("Error!\nError opening file. Exiting...\n");
		plFClose(memFile);
		return 1;
	}

	printf("Done\n");
	printf("Contents of %s:\n\n", filepath);
	while(plFGets(&stringBuffer, realFile) != 1){
		printf("%s", (char*)stringBuffer.data.pointer);
		for(int i = 0; i < 4096; i++)
			((char*)stringBuffer.data.pointer)[i] = 0;
		stringBuffer.data.size = 4096;
	}

	printf("Reading and writing to file-in-memory...");
	plstring_t memString = plRTStrFromCStr("test string getting sent to the yes\nnano", NULL);
	plFPuts(&memString, memFile);
	plFSeek(memFile, 0, SEEK_SET);
	printf("Done\n");
	printf("Contents of file-in-memory:\n");
	while(plFGets(&stringBuffer, memFile) != 1){
		printf("%s", (char*)stringBuffer.data.pointer);
		for(int i = 0; i < 4096; i++)
			((char*)stringBuffer.data.pointer)[i] = 0;
		stringBuffer.data.size = 4096;
	}

	plFClose(realFile);
	plFClose(memFile);

	return 0;
}

int testLoop(plstring_t strToTokenize, plmt_t* mt){
	plstring_t holder = {
		.data = {
			.pointer = NULL,
			.size = 0
		},
		.mt = NULL,
		.isplChar = false
	};
	plstring_t result = plRTTokenize(strToTokenize, &holder, mt);
	int i = 1;

	if(result.data.pointer == NULL)
		return 1;

	while(result.data.pointer != NULL){
		printf("Token %d: %s\n", i, (char*)result.data.pointer);
		plMTFree(mt, result.data.pointer);
		i++;
		result = plRTTokenize(holder, &holder, mt);
	}

	return 0;
}

int plRTTokenTest(plmt_t* mt){
	char* tknTestStrings[10] = { "oneword", "two words", "\"multiple words enclosed by quotes\" not anymore x3", "\"quotes at the beginning\" some stuff in the middle \"and now quotes at the back\"", "\"just quotes x3\"", "\'time for a literal string :3\' with stuff \"mixed all over\" it x3", "\"\\\"Escaped quotes this time\\\"\" and 'just a literal string with no ending :3", "\"now we have a basic string with no ending but 'a literal that does :3'", "string    with an  embedded = newline \" char\"\n  ", "[\"array test\",\'literal string here\', \"basic string here, preceded by a space\"]" };

	printf("This is a test of the pl-rt tokenizer\n\n");

	for(int i = 0; i < 10; i++){
		printf("Test %d:\n", i);
		if(testLoop(plRTStrFromCStr(tknTestStrings[i], NULL), mt)){
			printf("An error occurred. Exiting...\n");
			return 1;
		}
	}

	return 0;
}

int plMLTest(char* customFile, plmt_t* mt){
	printf("Parsing PLML...\n\n");
	char filepath[256] = "plml-test.plml";
	if(customFile != NULL)
		strncpy(filepath, customFile, 256);
	plfile_t* fileToParse = plFOpen(filepath, "r", mt);
	plstring_t lineBuffer = {
		.data = {
			.pointer = plMTAlloc(mt, 4096),
			.size = 4096
		},
		.mt = mt,
		.isplChar = false
	};

	int i = 1;
	while(plFGets(&lineBuffer, fileToParse) != 1){
		plmltoken_t parsedToken = plMLParse(lineBuffer, mt);

		printf("Token %d\n\n", i);
		printf("Name: %s\n", (char*)parsedToken.name.data.pointer);
		printf("Type: ");
		switch(parsedToken.type){
			case PLML_TYPE_STRING:
				printf("String\n");
				printf("Value: %s\n\n", (char*)parsedToken.value.string.pointer);
				break;
			case PLML_TYPE_BOOL:
				printf("Bool\n");
				printf("Value: ");
				if(parsedToken.value.boolean)
					printf("True\n\n");
				else
					printf("False\n\n");
				break;
			case PLML_TYPE_INT:
				printf("Integer\n");
				printf("Value: %li\n\n", parsedToken.value.integer);
				break;
			case PLML_TYPE_HEADER:
				printf("Header\n\n");
				break;
			case PLML_TYPE_FLOAT:
				printf("Float\n");
				printf("Value: %f\n\n", parsedToken.value.decimal);
				break;
			case PLML_TYPE_ARRAY:
				printf("Array\n");
				printf("Size: %ld\n", parsedToken.value.array.size);
				for(int i = 0; i < parsedToken.value.array.size; i++){
					plsimpletoken_t* rawArrayValues = parsedToken.value.array.pointer;
					printf("Value %d: ", i + 1);
					switch(rawArrayValues[i].type){
						case PLML_TYPE_STRING:
							printf("(String) %s\n", (char*)rawArrayValues[i].value.string.pointer);
							break;
						case PLML_TYPE_BOOL:
							printf("(Boolean) ");
							if(rawArrayValues[i].value.boolean)
								printf("True\n");
							else
								printf("False\n");
							break;
						case PLML_TYPE_INT:
							printf("(Integer) %li\n", rawArrayValues[i].value.integer);
							break;
						case PLML_TYPE_FLOAT:
							printf("(Float) %f\n", rawArrayValues[i].value.decimal);
							break;
						default: ;
					}
				}
				break;
			default: ;
		}

		plMLFreeToken(parsedToken);
		lineBuffer.data.size = 4096;
		i++;
	}

	plMTStop(mt);
	return 0;
}

int main(int argc, char* argv[]){
	plmt_t* mainMT = plMTInit(8 * 1024 * 1024);

	if(argc < 2){
		printf("Valid test values:\n memory-test\n file-test\n string-test\n token-test\n plml-test\n");
		return 1;
	}

	if(argc > 2)
		nonInteractive = true;

	if(strcmp(argv[1], "memory-test") == 0){
		return plRTMemoryTest(mainMT);
	}else if(strcmp(argv[1], "file-test") == 0){
		if(argc > 2)
			return plRTFileTest(argv[2], mainMT);

		return plRTFileTest(NULL, mainMT);
	}else if(strcmp(argv[1], "string-test") == 0){
		return plRTStringTest(mainMT);
	}else if(strcmp(argv[1], "token-test") == 0){
		return plRTTokenTest(mainMT);
	}else if(strcmp(argv[1], "plml-test") == 0){
		if(argc > 2)
			return plMLTest(argv[2], mainMT);

		return plMLTest(NULL, mainMT);
	}else{
		return 1;
	}
}
