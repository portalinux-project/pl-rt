******************************************
The PortaLinux Runtime v1.00 Documentation
******************************************

String Operations API
---------------------

This page contains documentation of all the public types and function definitions found in ``plrt-string.h``.

Custom Types
============

* ``plchar_t``: Struct containing an array of 4 bytes. Meant to represent a Unicode character in a way compatible with ASCII.
* ``plstring_t``: Struct containing a ``plptr_t``, a pointer to a memory tracker, and a boolean that confirms whether or not the ``plstring`` is a ``plchar_t`` array. This is the Runtime's way of having Rust-like strings within standard-compliant C99/C11. If the pointer to a memory tracker is NULL, it means that the string is not a dynamically allocated buffer.

Functions
=========

* ``plstring_t plRTStrFromCStr(char* cStr, plmt_t* mt)``: Converts a C string to a Runtime string. If ``mt`` is not NULL, it will make a dynamically allocated copy of ``cStr``.
* ``plstring_t plRTStrFromPLPtr(plptr_t pointer, plmt_t* mt, bool isplChar, bool isMemAlloc)``: Creates a Runtime string from a Runtime pointer. It will create a dynamically allocated copy of the pointer if ``mt`` is not NULL.
* ``void plRTStrCompress(plstring_t* plCharStr, plmt_t* mt)``: Compresses a ``plchar_t`` array into a UTF-8 C string.
* ``memptr_t plRTMemMatch(plptr_t memBlock1, plptr_t memBlock2)``: Searches for pattern ``memBlock2`` in ``memBlock1``. Returns a pointer to the first matching byte in ``memBlock1`` if there is a match, otherwise returns NULL.
* ``int plRTStrcmp(plstring_t string1, plstring_t string2)``: Compares two Runtime strings and returns 0 if they're the same. Otherwise returns a positive value if ``string1`` is greater than ``string2`` and a negative value if the pther way around.
* ``int64_t plRTStrchr(plstring_t string, plchar_t chr, size_t startAt)``: Searches for character ``chr`` in Runtime string ``string``. Returns the offset from ``string``'s raw pointer if found, -1 if not found.
* ``int64_t plRTStrstr(plstring_t string1, plstring_t string2, size_t startAt)``: Searches for pattern ``string2`` in Runtime string ``string1``. Returns the offset from ``string1``'s raw pointer if found, -1 if not found.
* ``plstring_t plRTStrtok(plstring_t string, plstring_t delimiter, plstring_t* leftoverStr, plmt_t* mt)``: Tokenizes a string based on the ``plchar_t`` array given through ``delimiter``. After dynamically allocating a return buffer with ``mt``, it puts the rest of the string in ``leftoverStr`` and returns a Runtime string containing the generated token.
* ``plstring_t plRTStrdup(plstring_t string, bool compress, plmt_t* mt)``: Duplicates Runtime string ``string`` by allocating a return buffer with ``mt``. If ``string`` is a ``plchar_t`` array, it compresses it into a UTF-8 C string if ``compress`` is set to true.
