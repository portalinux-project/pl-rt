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

int plMemoryTest(plmt_t* mt){
	printCurrentMemUsg(mt);

	printf("Allocating and initializing int array (4 ints)...");

	int* nano = plMTAllocE(mt, sizeof(int) * 4);
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

	char* nano2 = plMTAllocE(mt, sizeof(byte_t) * 16);
	char** nano3 = plMTAllocE(mt, sizeof(char*) * 4);
	int* nano4 = plMTAllocE(mt, sizeof(int) * 10);
	int* nano5 = plMTAllocE(mt, sizeof(int) * 20);

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

int main(){
	plmt_t* mainMT = plMTInit(8 * 1024 * 1024);
	return plMemoryTest(mainMT);
}
