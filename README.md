# pl-rt

The PortaLinux Runtime, pl-rt for short, is the core runtime of the PortaLinux
operating system. It defines the PortaLinux Runtime API, and it's the main
implementation of the PortaLinux Runtime API as defined in the ESB 2.0
Specification.

It is a heavily modified fork of pl32lib-ng release 1.06

# Build instructions

Currently, it imports the old POSIX shell-based pl32lib-ng build system, but I
may bring back the Meson build system files.

To configure the install path and the system to target, run the following:
```sh
./configure
```
(To see all of the options you can change, run `./configure --help`).

To compile and install the project, run the following:
```sh
./compile build
./compile install     # You may need to run this as superuser
```

# Contributions

Since this is a fork of pl32lib-ng, it inherits its no contribution rule. I,
Sophie, will be the only contributor to the project throughout its life.
