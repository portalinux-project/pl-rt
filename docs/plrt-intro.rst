******************************************
The PortaLinux Runtime v1.00 Documentation
******************************************

The PortaLinux Runtime is the runtime library for the PortaLinux Operating System, typically used for its init system.

The Runtime consists of a memory tracker, a string tokenizer, Rust-style strings with Unicode support, memory-backed
files , a logger, and some other miscellaneous POSIX routines. It only depends on a compiler that supports C99 and a C
Standard Library that supports both C11 and POSIX-2008.

Documentation Pages
###################

* `Types and Error Handling API`_
* `Memory Tracker API`_
* `String Operations API`_
* `File Operations API`_
* `Tokenizer API`_
* `Logger API`_
* `Misc POSIX API`_

For more in-depth documentation, go to https://cinnamonwolfy.neocities.org/docs/pl-rt

.. _`Types and Error Handling API`: plrt-types.rst
.. _`Memory Tracker API`: plrt-memory.rst
.. _`String Operations API`: plrt-string.rst
.. _`File Operations API`: plrt-file.rst
.. _`Tokenizer API`: plrt-token.rst
.. _`Logger API`: plrt-logger.rst
.. _`Misc POSIX API`: plrt-misc-posix.rst
