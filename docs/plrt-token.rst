******************************************
The PortaLinux Runtime v1.00 Documentation
******************************************

Tokenizer API
-------------

This page contains documentation of all the public types and function definitions found in ``plrt-token.h``.

Functions
=========

* ``plstring_t plRTTokenize(plstring_t string, plstring_t* leftoverStr, plmt_t* mt)``: Generate a token from a string. It generates tokens in a similar way that the Unix shell does. Returns generated token and puts the remaining string in ``leftoverStr``
* ``plptr_t plRTParser(plstring_t input, plmt_t* mt)``: Fully tokenizes a string. Internally uses ``plRTTokenize`` in a loop until a string is fully tokenized. Returns a ``plstring_t`` array.
* ``void plRTFreeParsedString(plptr_t stringArray)``: Frees a ``plstring_t`` array. Usually these arrays are from ``plRTParser``.