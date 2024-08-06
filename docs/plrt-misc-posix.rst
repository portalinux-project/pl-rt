******************************************
The PortaLinux Runtime v1.03 Documentation
******************************************

Misc POSIX API
--------------

This page contains documentation of all non-logging function definitions found in ``plrt-posix.h``.

Functions
=========

* ``void plRTSetSignal(int signal)``: Change the handling function for the given signal to ``signalHandler``, which has to be enabled explicitly.
* ``pid_t plRTSpawn(plptr_t args)``: Spawns a process with the given arguments.
* ``plptr_t plRTGetDirents(char* path, plmt_t* mt)``: Gets a list of directory entry structures (``struct dirent``). The list is unsorted.
* ``void plRTSortDirents(plptr_t direntArray)``: Sorts a directory entry list. 
