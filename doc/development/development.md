# Development
This document covers various aspects of developing for pumptools. For developing using the API, see 
[the dedicated readme](../api/api.md).

## Architecture
[This document](architecture.md) gives an introduction to the architecture of pumptools. This helps you to get familiar
with the codebase and its features, definitely recommended to read this first.

## Development environment setup

Pumptools targets the Linux-based Piu games. Therefore, you need a 64-bit Linux distribution of
your choice.

Pumptools is written entirely in C and built using cmake. Various helper scripts are written as bash/shell scripts.
Therefore, you need the following tools on your development setup:
* gcc
* cmake
* make
* bash or compatible shell
* clang-format

Pumptools requires the following dependencies to be installed on your development OS:
* libcurl4 i386
* libusb 1.0 i386

Furthermore, header files, which come with the development packages of the libs, of the following libraries are
required:
* fmodex
* libconfig
* X11
* libalsa
* libusb-0.1 (**not** 1.0)

The project does not depend on any specific IDE. However, if you use CLion, the project can be imported easily.

## Code style
The main style guidelines are defined by the `.clang-format` file and the code style can be easily applied to the
whole `src` directory using the following command from the root folder:
```shell script
make clang-format
```

Make sure that your contributions follow the style and apply it before submitting your changes.

For other source files like shell scripts, cmake, makefile or markdown documents, please follow the already established
style in the existing documents.

## Building
See the [main readme](../../README.md#building).

## Compatibility issues with newer glibc versions
We have experienced various issues with different glibc versions which had a major impact on the builds. The following
table gives you a list of tested glibc versions and issues that are either present or not with each version listed.
"ok" indicates that the specified glibc version is compatible and the specific issue does not occur, "bad" indicates
incompatibility and that the specific issue does occur when using a binary compiled with that glibc version.

| Issue/glibc version | 2.23 | 2.30 |
|---------------------|------|------|
| Issue 1             |  ok  |  bad |
| Issue 2             |  ok  |  bad |

The issues are further described in the following sub-sections

### Issue 1: Crash during static initialization
For a detailed report on this issue, see the [dedicated notes](notes/f2-crashing-on-modern-linux-post-mortem.md).

Summarized: Fiesta 2 and newer will segfault before even running the application's main method. The application fails
on runtime initialization when initializing the static context. This is caused by ABI incompatibility of inlined code
of C++'s STL, first occurring with string allocations during static initialization.

### Issue 2: sscanf not being detoured
With NX2, Andamiro locked the USB drives to be used to a specific brand which they sold back then. That lock is patched
out by detouring `sscanf` and manipulating the VID/PID and serial number parsing logic in the game to always return
the values, the game wants to see to accept the connected USB drive.

However, some incompatibility on newer glibc versions does not setup the `sscanf` detour properly. The detour function
is there and gets called by various parts of the game code in the beginning, but later calls to `sscanf` always go
to the real function instead. Thus, the usb unlock feature does not work.

## Unit tests
Unfortunately, this was started very late in the development of pumptools, but it's not too late to keep extending this
with every change to ensure existing features don't break and future ones work.

When you have a change to commit, check if there is an existing unit-test which needs to be aligned with your change(s).
If not, create one.

For testing code that gets hooked, there is a specific pattern that needs to be applied in order to allow testing
hooks without having to actually hook them. As an example, you can take a look at the 
[usb-fix module](../../src/main/hook/propatch/usb-fix.h) for piu pro and the 
[corresponding test](../../src/test/hook/propatch/usb-fix/main.c). It shows you how to setup the hooks and create
mocks for the functions that you can trap the calls to the real detoured functions to check if they are getting called
or return fixed values for testing.

## Debugging
### Pumptool's built-in tooling
Pumptools has a few basic debugging features built in which helps analyzing common issues very quickly without the
need of external tools.

#### Help and usage message of hook library
When running a game with any of pumptools's `hook.so` libs attached, you can always add the parameter `-h` to display a
help/usage message with all available command line and configuration parameters.

For example, when using `piueb` for bootstrapping:
```
./piueb run -h
```

The `-h` is passed on as a parameter and appended to the execution command. You can make use of that with other
available cmd parameters as well (see usage information).

#### Logging
Logging to console and file as well as the level of verbosity can be controlled using the `hook.conf` configuration file
or via command line arguments provided on execution.

All hooks implement various sanity checks for parameters provided, file checks etc. so there is a good coverage of
common mistakes and misconfiguration that can be easily detected by watching out for any ERROR or WARNING level messages
showing up in the log.

Example in configuration file:
```
util.log.file=pumptools.log
util.log.level=4
```

Furthermore, `piueb` outputs a separate log file `piueb.log` which contains logging data of the environment setup
process of `piueb`.

#### File and IO hook monitoring
In the configuration file or via cmd args, you can enable monitoring of file, filesystem, usb and/or open system calls
for tracing and debugging. This can be useful, to check if the game cannot find certain files like game assets.

Example in configuration file:
```
patch.hook_mon.file=0
patch.hook_mon.fs=0
patch.hook_mon.io=0
patch.hook_mon.open=1
patch.hook_mon.usb=0
```

#### Halt on SIGSEGV and attach debugger
Also enabled via configuration or cmd args, the application is halted on SIGSEGV to allow attaching a debugger.

Example in configuration file:
```
patch.sigsegv.halt_on_segv=1
```

### External tooling
#### Remote debugging
You can run the application with `gdbserver` using `piueb`:
```shell script
./piueb debug
```

And connect with your favorite debugger (e.g. IDA).

#### strace
You can run the application with `strace` using `piueb`:
```shell script
./piueb strace
```

#### valgrind
You can run the application with `valgrind` using `piueb`:
```shell script
./piueb valgrind
```