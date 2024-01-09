******************************************
The PortaLinux Runtime v1.00 Documentation
******************************************

File Operations API
-------------------

This page contains documentation of all the public types and function definitions found in ``plrt-file.h``.

Custom Types
============

* ``plfile_t``: Structure containing information about a file. It can either be a physical file on a storage file, or an internal dynamically allocated memory buffer that expands automatically. This is the Runtime's way of having GNU libc's memory buffer as a file stream feature.

Functions
=========

* ``plfile_t* plFOpen(char* filename, char* mode, plmt_t* mt)``: Opens a file located at ``filename``. Returns a Runtime file handle. If ``filename`` is NULL, it will return a file handle to a dynamically allocated internal buffer buffer.
* ``plfile_t* plFToP(FILE* pointer, plmt_t* mt)``: Wraps a Standard C file handle in a Runtime file handle. This is done for compatibility reasons.
* ``int plFClose(plfile_t* ptr)``: Closes a file handle.
* ``size_t plFRead(plptr_t ptr, plfile_t* stream)``: Read the amount of bytes specified in ``ptr.size``, and copy them to the buffer pointed to by ``ptr.pointer``. Returns the amount of bytes read, usually the size of the buffer specified by ``ptr`` or until it reaches the end of the file.
* ``size_t plFWrite(plptr_t ptr, plfile_t* stream)``: Write the buffer specified in ``ptr``. Returns the amount of bytes written to the stream, usually the size of the buffer given.
* ``int plFPutC(byte_t ch, plfile_t* stream)``: Write a singular byte to the stream. Returns 1 on failure.
* ``int plFGetC(plfile_t* stream)``:  Gets a singular byte from the stream. Returns the character read.
* ``int plFPuts(plstring_t string, plfile_t* stream)``: Writes a string to the stream. Returns 1 on failure
* ``int plFGets(plstring_t* string, plfile_t* stream)``: Gets a string from the stream and copies it to the given buffer. Returns 1 on failure.
* ``int plFSeek(plfile_t* stream, long int offset, int whence)``: Moves file position indicator by ``offset``, relative to either the beginning of the file, end of the file, or current position.
* ``size_t plFTell(plfile_t* stream)``: Returns the current position of the position indicator.
* ``void plFFlush(plfile_t* stream)``: Commits any changes to the physical file location. Does nothing on memory buffers.

