/****************************************************\
 pl32lib-ng, v1.05
 (c) 2022 pocketlinux32, Under MPL v2.0
 pl32-file.c: File management module
\****************************************************/
#include <pl32-file.h>

struct plfile {
	FILE* fileptr; /* File pointer for actual files */
	byte_t* strbuf; /* String pointer for stringstream */
	size_t seekbyte; /* Byte offset from the beginning of buffer */
	size_t bufsize; /* Buffer size */
	plmt_t* mtptr; /* pointer to MT (see pl32-memory.h) */
};

/* Opens a file stream. If filename is NULL, a file-in-memory is returned */
plfile_t* plFOpen(string_t filename, string_t mode, plmt_t* mt){
	if(mt == NULL)
		plPanic("plFOpen: Memory tracker was set to NULL", false, true);

	plfile_t* returnStruct = plMTAllocE(mt, sizeof(plfile_t));

	/* If no filename is given, set up a file in memory */
	if(filename == NULL){
		returnStruct->fileptr = NULL;
		returnStruct->strbuf = plMTAllocE(mt, 4098);
		returnStruct->bufsize = 4098;
	}else{
		if(mode == NULL)
			plPanic("plFOpen: File mode was set to NULL", false, true);

		returnStruct->fileptr = fopen(filename, mode);
		returnStruct->bufsize = 0;
		returnStruct->strbuf = NULL;

		if(returnStruct->fileptr == NULL){
			plPanic("plFOpen", true, false);
		}
	}

	returnStruct->mtptr = mt;
	returnStruct->seekbyte = 0;


	return returnStruct;
}

/* Converts a FILE pointer into a plfile_t pointer */
plfile_t* plFToP(FILE* pointer, string_t mode, plmt_t* mt){
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
size_t plFRead(void* ptr, size_t size, size_t nmemb, plfile_t* stream){
	if(stream == NULL)
		return 0;

	if(stream->fileptr == NULL){
		int elementAmnt = 1;
		while(size * elementAmnt < stream->bufsize - stream->seekbyte && elementAmnt < nmemb){
			elementAmnt++;
		}
		elementAmnt--;

		if(elementAmnt == 0){
			return 0;
		}

		memcpy(ptr, stream->strbuf + stream->seekbyte, size * elementAmnt);
		stream->seekbyte += size * elementAmnt;
		return size * elementAmnt;
	}else{
		return fread(ptr, size, nmemb, stream->fileptr);
	}
}

/* Writes size * nmemb amount of bytes from the file stream */
size_t plFWrite(void* ptr, size_t size, size_t nmemb, plfile_t* stream){
	if(stream == NULL)
		return 0;

	if(stream->fileptr == NULL){
		if(size * nmemb > stream->bufsize - stream->seekbyte){
			void* tempPtr = plMTRealloc(stream->mtptr, stream->strbuf, stream->bufsize + size * nmemb);
			if(!tempPtr){
				return 0;
			}

			stream->strbuf = tempPtr;
		}
		memcpy(stream->strbuf + stream->seekbyte, ptr, size * nmemb);
		stream->seekbyte += size * nmemb;
		return size * nmemb;
	}else{
		return fwrite(ptr, size, nmemb, stream->fileptr);
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
int plFPuts(string_t string, plfile_t* stream){
	if(stream == NULL)
		return 0;

	if(stream->fileptr == NULL){
		if(plFWrite(string, 1, strlen(string)+1, stream))
			return 0;

		return 1;
	}else{
		return fputs(string, stream->fileptr);
	}
}

/* Gets a string from the file stream */
string_t plFGets(string_t string, int num, plfile_t* stream){
	if(stream == NULL)
		return NULL;

	if(stream->fileptr == NULL){
		byte_t* endMark = (byte_t*)strchr((string_t)stream->strbuf + stream->seekbyte, '\n');
		unsigned int writeNum = 0;
		if(endMark == NULL)
			endMark = (byte_t*)strchr((string_t)stream->strbuf + stream->seekbyte, '\0');

		writeNum = endMark - (stream->strbuf + stream->seekbyte);

		if(writeNum >= num)
			writeNum = num - 1;

		if(writeNum == 0)
			return NULL;

		memcpy(string, stream->strbuf + stream->seekbyte, writeNum);
		string[writeNum] = '\n';

		if(stream->seekbyte + writeNum + 1 > stream->bufsize){
			stream->seekbyte = stream->bufsize - 1;
		}else{
			stream->seekbyte += (writeNum + 1);
		}

		return string;
	}else{
		return fgets(string, num, stream->fileptr);
	}
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

/* Converts a memory buffer into a physical file */
int plFPToFile(string_t filename, plfile_t* stream){
	if(stream == NULL || filename == NULL || stream->strbuf == NULL)
		plPanic("plFPToFile: Stream, filename and/or byte buffer is NULL", false, true);

	FILE* realFile = fopen(filename, "w");
	if(realFile == NULL)
		plPanic("plFPToFile", true, false);

	int retVar = fputs((string_t)stream->strbuf, realFile);
	fclose(realFile);
	return retVar;
}

/* Concatenates two files */
void plFCat(plfile_t* dest, plfile_t* src, int destWhence, int srcWhence, bool closeSrc){
	if(dest == NULL || src == NULL)
		plPanic("plFCat: Destination and/or source stream is NULL", false, true);

	plFSeek(dest, 0, destWhence);
	plFSeek(src, 0, srcWhence);
	byte_t ch;

	while((ch = plFGetC(src)) != '\0')
		plFPutC(ch, dest);

	if(closeSrc)
		plFClose(src);
}
