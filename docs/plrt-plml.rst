******************************************
The PortaLinux Runtime v1.02 Documentation
******************************************

PLML Parser API
---------------

This page contains documentation of all the public types and function definitions found in ``plrt-plml.h``.
(PLML Standard: https://github.com/cinnamonwolfy/cinnamonwolfy/blob/main/plml.rst)

Custom Types
============

* ``plmltype_t``: An enum containing all of the types a value in PLML can be
	* ``PLML_TYPE_NULL``: No value
	* ``PLML_TYPE_INT``: Integer value
	* ``PLML_TYPE_BOOL``: Boolean value
	* ``PLML_TYPE_FLOAT``: Floating-point value
	* ``PLML_TYPE_STRING``: String value
* ``plmlval_t`: Holds variable value
* ``plsimpletoken_t``: Simplified token structure used internally during the token parsing process
* ``plmltoken_t``: The actual token structure returned by ``plMLParse``

Functions
=========

* ``plmltoken_t plMLParse(plstring_t string, plmt_t* mt)``: Parses a string into a token struct
* ``void plMLFreeToken(plmltoken_t token)``: Frees any dynamically allocated memory used in the token struct
* ``plstring_t plMLGenerateTokenStr(plmltoken_t token, plmt_t* mt)``: Generates a TOML-compatible PLML token string
