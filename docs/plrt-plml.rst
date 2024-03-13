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
* ``plmlval_t`: Holds 
