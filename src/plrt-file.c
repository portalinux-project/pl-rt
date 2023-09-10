/****************************************************\
 pl-rt, v0.04
 (c) 2022-2023 pocketlinux32, Under MPL v2.0
 plrt-file.c: File management module
\****************************************************/
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
		returnStruct->strbuf = plMTAlloc(mt, 4096);
		returnStruct->bufsize = 4096;
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
int plFClose(plfile_t* ptr){
	if(ptr == NULL)
		return 1;

	if(ptr->fileptr == NULL){
		plMTFree(ptr->mtptr, ptr->strbuf);
	}else{
		if(fclose(ptr->fileptr))
			return 1;
	}

	plMTFree(ptr->mtptr, ptr);
	return 0;
}

/* Reads size * nmemb amount of bytes from the file stream */
size_t plFRead(plptr_t ptr, plfile_t* stream){
	if(stream == NULL || ptr.pointer == NULL)
		plRTPanic("plFRead", PLRT_ERROR | PLRT_NULL_PTR, true);

	if(stream->fileptr == NULL){
		if(ptr.size > stream->bufsize - stream->seekbyte)
			return 0;

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

	if(stream->fileptr == NULL){
		if(ptr.size > stream->bufsize - stream->seekbyte){
			void* tempPtr = plMTRealloc(stream->mtptr, stream->strbuf, stream->bufsize + ptr.size);
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

	if(stream->fileptr == NULL){
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

	if(stream->fileptr == NULL){
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
int plFPuts(plstring_t* string, plfile_t* stream){
	if(stream == NULL || string == NULL || string->data.pointer == NULL)
		plRTPanic("plFPuts", PLRT_ERROR | PLRT_NULL_PTR, true);

	if(stream->fileptr == NULL){
		if(plFWrite(string->data, stream))
			return 0;

		return 1;
	}else{
		return fputs(string->data.pointer, stream->fileptr);
	}
}

/* Gets a string from the file stream */
int plFGets(plstring_t* string, plfile_t* stream){
	if(stream == NULL || string == NULL || string->data.pointer == NULL)
		plRTPanic("plFGets", PLRT_ERROR | PLRT_NULL_PTR, true);

	if(stream->fileptr == NULL){
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
	}

	return 0;
}

/* Moves the seek position offset amount of bytes relative from whence */
int plFSeek(plfile_t* stream, long int offset, int whence){
	if(stream == NULL)
		return 1;

	if(stream->fileptr == NULL){
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

	if(stream->fileptr == NULL){
		return stream->seekbyte;
	}else{
		fflush(stream->fileptr);
		return ftell(stream->fileptr);
	}
}
