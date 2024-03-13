******************************************
The PortaLinux Runtime v1.02 Documentation
******************************************

Memory Tracker API
------------------

This page contains documentation of all the public types and function definitions found in ``plrt-memory.h``.

Custom Types
============

* ``plmt_t``: A structure containing the current state of the memory tracker. Always in the form of a pointer.
* ``plmtaction_t``: An enum containing action codes to get or set different values in the memory tracker. All of the action codes are as follows:
    * ``PLMT_GET_USEDMEM``: Gets the currently allocated amount of memory in bytes
    * ``PLMT_GET_MAXMEM``: Gets the maximum amount of memory that can be allocated in the memory tracker in bytes
    * ``PLMT_SET_MAXMEM``: Sets the new maximum amount of memory that can be allocated

Functions
=========

* ``plmt_t* plMTInit(size_t maxMemoryAlloc)``: Starts an instance of the memory tracker of the specified amount in bytes. A size of zero (0) will set 8 MiB by default.
* ``void plMTStop(plmt_t* mt)``: Deallocates all pointer entries and the memory tracker itself.
* ``memptr_t plMTAlloc(plmt_t* mt, size_t size)``: Allocates a block of memory and adds the pointer to the memory tracker. Returns a pointer to the dynamically allocated block of memory.
* ``memptr_t plMTRealloc(plmt_t* mt, memptr_t pointer, size_t size)``: Resizes an already allocated block to ``size`` in bytes.
* ``void plMTFree(plmt_t* mt, memptr_t pointer)``: Frees an allocated block of memory from the memory tracker.

