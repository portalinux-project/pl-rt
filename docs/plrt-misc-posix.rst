******************************************
The PortaLinux Runtime v1.00 Documentation
******************************************

Misc POSIX API
--------------

This page contains documentation of all non-logging function definitions found in ``plrt-posix.h``.

Functions
=========

* ``void plRTSetSignal(int signal)``: Change the handling function for the given signal to ``signalHandler``, which must be included in all PortaLinux Runtime applications.
* ``pid_t plRTSpawn(plptr_t args)``: Spawns a process with the given arguments.
* ``plptr_t plRTGetDirents(char* path, plmt_t* mt)``: Gets an alphabetically organized list of file entries from the given directory.
