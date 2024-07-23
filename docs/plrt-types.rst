******************************************
The PortaLinux Runtime v1.03 Documentation
******************************************

Types and Error Handling API
----------------------------

This page contains documentation of all the public types and function definitions found in ``plrt-types.h``. 

Macros
======

* ``PLRT_VERSION``: A string containing the version number.
* ``PLRT_API_VER``: An integer containing the API version (If it increases, it means the API has breaking changes).
* ``PLRT_FEATURELVL``: An integer containing the amount of feature updates to the API.
* ``PLRT_PATCHLVL``: An integer containing the amount of patch updates to the feature update.
* ``UINT_SHIFT_LIMIT``: A constant to shift to the outermost bit in an unsigned 64-bit integer (``uint64_t``).

Custom Types
============

* ``byte_t``: An unsigned byte, typedef'd to ``uint8_t``.
* ``memptr_t``: A generic memory pointer, typedef'd to ``void*``.
* ``plptr_t``: A struct meant to be a Rust-like representation of a pointer. It consists of members ``pointer`` and ``size``
* ``plrtret_t``: An enum representing all Runtime-specific errors. All valid error codes must have ``PLRT_ERROR`` bitwise OR'd into the code. All of the errors are as follows:
    * ``PLRT_OOM``: All of the memory in the memory tracker has been exhausted
    * ``PLRT_FAILED_ALLOC``: Falied to allocate memory
    * ``PLRT_INVALID_PTR``: Invalid pointer address
    * ``PLRT_NULL_PTR``: The pointer given was a NULL pointer
    * ``PLRT_ACCESS``: Insufficient permissions to open a file
    * ``PLRT_INVALID_MODE``: Invalid file opening mode
    * ``PLRT_ALREADY_EXISTS``: File exists and cannot be overwritten
    * ``PLRT_NO_EXIST``: File doesn't exist
    * ``PLRT_IOERR``: Generic I/O error
    * ``PLRT_NOT_DIR``: Not a directory
    * ``PLRT_NOT_FILE``: Is a directory
    * ``PLRT_NOT_PLCHAR``: Runtime String is not in plchar format
    * ``PLRT_NOT_COMPRESSED``: Runtime String is not in compressed/raw format
    * ``PLRT_MATCH_NOT_FOUND``: Pattern could not be matched to the string given
    * ``PLRT_TOKENIZER_FAILURE``: Tokenizer could not generate a token out of given string
    * ``PLRT_TOKENIZER_WTF``: Either the system is next to an EMP or you're messing around with a debugger (if that's the case, get back to work dingus x3)
    * ``PLRT_INVALID_TOKEN``: Invalid PLML token

Functions
=========

* ``void plRTErrorString(plptr_t* buffer, plrtret_t errCode)``: Copies a pointer to an internal static error string based in ``errCode`` into ``buffer``. Meant to be used for printing out errors.
* ``void plRTPanic(char* msg, plrtret_t errCode, bool isDeveloperBug)``: Prints out ``msg``, an error string gotten from ``plRTErrorString`` and then a bug disclaimer if ``isDeveloperBug`` is true. It sends an abort signal to the program and terminates the process.
