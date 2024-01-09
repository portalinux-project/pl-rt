******************************************
The PortaLinux Runtime v1.00 Documentation
******************************************

Logger API
----------

This page contains documentation of the logging-related types and functions in ``plrt-posix.h``.

Custom Types
============

* ``plloglevel_t``: An enum containing the logging levels supported by the Runtime logger. Here are the values:
    * ``LOG_DEBUG``: Signals to the logger that this is a debug entry
    * ``LOG_INFO``: Signals to the logger that this is a general info entry 
    * ``LOG_WARNING``: Signals to the logger that this is a warning entry 
    * ``LOG_ERROR``: Signals to the logger that this is an error entry 
    * ``LOG_FATAL_ERROR``: Signals to the logger that this is a fatal error entry 

Functions
=========

* ``plfile_t* plRTLogStart(char* prefix, plmt_t* mt)``: Creates a log file in /var/log/``prefix``.
* ``void plRTLog(plfile_t* logFile, plloglevel_t logLevel, plstring_t string)``: Writes a log entry to the log file.
* ``void plRTLogStop(plfile_t* logFile)``: Closes the file and flushes all entries.
