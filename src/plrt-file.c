/*******************************************\
 pl-rt, v1.02
 (c) 2024 CinnamonWolfy, Under MPL v2.0
 plrt-file.c: File management module
\*******************************************/
#include <plrt-file.h>
#include <errno.h>

struct plfile {
	FILE* fileptr; /* File pointer for actual files */
	byte_t* strbuf; /* String pointer for stringstream */
	size_t seekbyte; /* Byte offset from the beginning of buffer */
	size_t bufsize; /* Buffer size */
	plmt_t* mtptr; /* pointer to Memory Tracker (see plrt-memory.h) */
};

/* Opens a file stream. If filename is NULL, a file-in-memory is returned */
plfile_t* plFOpen(char* filename, char* mode, plmt_t* mt){
	if(mt == NULL)
		plRTPanic("plFOpen", PLRT_ERROR | PLRT_NULL_PTR, true);

	plfile_t* returnStruct = plMTAlloc(mt, sizeof(plfile_t));

	/* If no filename is given, set up a file in memory */
	if(filename == NULL){
		returnStruct->fileptr = NULL;
		returnStruct->strbuf = plMTAlloc(mt, 65536);
		returnStruct->bufsize = 65536;
	}else{
		if(mode == NULL)
			plRTPanic("plFOpen", PLRT_ERROR | PLRT_NULL_PTR, true);

		returnStruct->fileptr = fopen(filename, mode);
		returnStruct->bufsize = 0;
		returnStruct->strbuf = NULL;

		if(returnStruct->fileptr == NULL){
			plRTPanic("plFOpen", PLRT_ERROR | PLRT_ERRNO | errno, false);
		}
	}

	returnStruct->mtptr = mt;
	returnStruct->seekbyte = 0;


	return returnStruct;
}

/* Converts a FILE pointer into a plfile_t pointer */
plfile_t* plFToP(FILE* pointer, plmt_t* mt){
	if(pointer == NULL)
		return NULL;

	plfile_t* returnPointer = plFOpen(NULL, NULL, mt);
	returnPointer->fileptr = pointer;
	returnPointer->bufsize = 0;
	plMTFree(mt, returnPointer->strbuf);
	return returnPointer;
}

/* Closes a file stream */
int plFClose(plfile_t* stream){
	if(stream == NULL)
		return 1;

	if(stream->strbuf != NULL)
		plMTFree(stream->mtptr, stream->strbuf);

	if(stream->fileptr != NULL){
		if(fclose(stream->fileptr))
			return 1;
	}

	plMTFree(stream->mtptr, stream);
	return 0;
}

/* Reads size * nmemb amount of bytes from the file stream */
size_t plFRead(plptr_t ptr, plfile_t* stream){
	if(stream == NULL || ptr.pointer == NULL)
		plRTPanic("plFRead", PLRT_ERROR | PLRT_NULL_PTR, true);

	if(stream->strbuf != NULL){
		if(ptr.size > stream->bufsize - stream->seekbyte){
			if(stream->bufsize - stream->seekbyte)
				return 0;
			ptr.size = stream->bufsize - stream->seekbyte;
		}

		memcpy(ptr.pointer, stream->strbuf + stream->seekbyte, ptr.size);
		stream->seekbyte += ptr.size;
		return ptr.size;
	}else{
		return fread(ptr.pointer, ptr.size, 1, stream->fileptr);
	}
}

/* Writes size * nmemb amount of bytes from the file stream */
size_t plFWrite(plptr_t ptr, plfile_t* stream){
	if(stream == NULL || ptr.pointer == NULL)
		plRTPanic("plFWrite", PLRT_ERROR | PLRT_NULL_PTR, true);

	if(stream->strbuf != NULL){
		if(ptr.size > stream->bufsize - stream->seekbyte){
			void* tempPtr = plMTRealloc(stream->mtptr, stream->strbuf, stream->seekbyte + ptr.size + 1);
			if(tempPtr == NULL)
				return 0;

			stream->strbuf = tempPtr;
		}
		memcpy(stream->strbuf + stream->seekbyte, ptr.pointer, ptr.size);
		stream->seekbyte += ptr.size;
		return ptr.size;
	}else{
		return fwrite(ptr.pointer, ptr.size, 1, stream->fileptr);
	}
}

/* Puts a character into the file stream */
int plFPutC(byte_t ch, plfile_t* stream){
	if(stream == NULL)
		return '\0';

	if(stream->strbuf != NULL){
		if(stream->bufsize - stream->seekbyte < 1){
			void* tempPtr = plMTRealloc(stream->mtptr, stream->strbuf, stream->bufsize + 1);

			if(tempPtr == NULL)
				return '\0';

			stream->strbuf = tempPtr;
		}

		return ch;
	}else{
		return fputc(ch, stream->fileptr);
	}
}

/* Gets a character from the file stream */
int plFGetC(plfile_t* stream){
	if(stream == NULL)
		return '\0';

	if(stream->strbuf != NULL){
		byte_t ch = '\0';
		if(stream->seekbyte > stream->bufsize){
			ch = *(stream->strbuf + stream->seekbyte);
			stream->seekbyte++;
		}

		return ch;
	}else{
		return fgetc(stream->fileptr);
	}
}

/* Puts a string into the file stream */
int plFPuts(plstring_t string, plfile_t* stream){
	if(stream == NULL || string.data.pointer == NULL)
		plRTPanic("plFPuts", PLRT_ERROR | PLRT_NULL_PTR, true);

	if(stream->strbuf != NULL){
		if(plFWrite(string.data, stream))
			return 0;

		return 1;
	}else{
		return fputs(string.data.pointer, stream->fileptr);
	}
}

/* Gets a string from the file stream */
int plFGets(plstring_t* string, plfile_t* stream){
	if(stream == NULL || string == NULL || string->data.pointer == NULL)
		plRTPanic("plFGets", PLRT_ERROR | PLRT_NULL_PTR, true);

	if(stream->strbuf != NULL){
		byte_t* endMark = (byte_t*)strchr((char*)stream->strbuf + stream->seekbyte, '\n');
		unsigned int writeNum = 0;
		if(endMark == NULL)
			endMark = (byte_t*)strchr((char*)stream->strbuf + stream->seekbyte, '\0');

		writeNum = endMark - (stream->strbuf + stream->seekbyte);

		if(writeNum >= string->data.size)
			writeNum = string->data.size - 1;

		if(writeNum == 0)
			return 1;

		memcpy(string->data.pointer, stream->strbuf + stream->seekbyte, writeNum);
		((char*)string->data.pointer)[writeNum] = '\n';

		if(stream->seekbyte + writeNum + 1 > stream->bufsize){
			stream->seekbyte = stream->bufsize - 1;
		}else{
			stream->seekbyte += (writeNum + 1);
		}
	}else{
		memptr_t tmpVar = fgets(string->data.pointer, string->data.size, stream->fileptr);
		if(tmpVar == NULL)
			return 1;
		string->data.size = strlen(string->data.pointer);
	}

	return 0;
}

/* Moves the seek position offset amount of bytes relative from whence */
int plFSeek(plfile_t* stream, long int offset, int whence){
	if(stream == NULL)
		return 1;

	if(stream->strbuf != NULL){
		switch(whence){
			case SEEK_SET:
				if(offset < stream->bufsize){
					stream->seekbyte = offset;
				}else{
					return 1;
				}
				break;
			case SEEK_CUR:
				if(stream->seekbyte + offset < stream->bufsize){
					stream->seekbyte += offset;
				}else{
					return 1;
				}
				break;
			case SEEK_END:
				if(stream->bufsize - offset >= 0){
					stream->seekbyte = stream->bufsize - offset;
				}else{
					return 1;
				}
				break;
			default:
				return 1;
		}

		return 0;
	}else{
		return fseek(stream->fileptr, offset, whence);
	}
}

/* Tells you the current seek position */
size_t plFTell(plfile_t* stream){
	if(stream == NULL)
		return 0;

	if(stream->strbuf != NULL){
		return stream->seekbyte;
	}else{
		fflush(stream->fileptr);
		return ftell(stream->fileptr);
	}
}

/* Flushes a file stream */
void plFFlush(plfile_t* stream){
	if(stream->fileptr != NULL)
		fflush(stream->fileptr);
}

/* Loads entire file into memory */
void plFLoad(plfile_t* stream){
	if(stream->fileptr == NULL)
		return;

	freopen(NULL, "r", stream->fileptr);
	fseek(stream->fileptr, 0, SEEK_SET);
	char buffer[4096] = "";
	plptr_t pointerStruct = {
		.pointer = NULL,
		.size = 0
	};

	stream->strbuf = plMTAlloc(stream->mtptr, 65536);
	stream->bufsize = 65536;

	if(fgets(buffer, 4096, stream->fileptr) != NULL){
		pointerStruct.pointer = buffer;
		pointerStruct.size = strlen(buffer);
		plFWrite(pointerStruct, stream);

	}
}

/* Writes file-in-memory into physical file */
void plFUnload(plfile_t* stream){
	if(stream->fileptr == NULL || stream->strbuf == NULL)
		return;

	freopen(NULL, "w", stream->fileptr);
	fseek(stream->fileptr, 0, SEEK_SET);
	if(fwrite(stream->strbuf, stream->seekbyte + 1, 1, stream->fileptr) == -1)
		plRTPanic("plFUnload", PLRT_ERROR | PLRT_IOERR, false);

	plMTFree(stream->mtptr, stream->strbuf);
}
