# Notable features

* Runs on recent kernel versions thanks to various fixes
* Runs on 32-bit and 64-bit distros (64-bit distros require additional 32-bit libs to be installed)
* Full dongle emulation
* Remove HDD checks to run this on "non legit" drives
* Full MK6IO emulation with API hook: Keyboard or your own custom IO
* Real IO passthrough (for MK6 usb io)

# Versions supported
All known versions supported.

# Data setup
You are expected to get a clean set of data from a prestine drive. Ensure that
the game version matches one of the supported versions listed.

You need two main folders:
* data
* settings

## Data folder
Contents of the folder:
* game: The binaries of the custom "Fiesta FS". These are stored in raw areas
of the HDD and have to be extraced. File names must be lower case.
* lib: Put any libraries (especially older versions of libraries that can't be
installed anymore using the package manager) the game uses and aren't installed
on your system in here.
* piu: The piu executable

## Settings folder
The contents of the folder are auto generated if the files don't exist.
Otherwise, this folder contains:
* PIUFESTAEX.INI
* RANK.DATA

## Executable dependencies
All dependencies must be compiled as 32-bit binaries. Here is a list of
dependencies (with versions) required to run the game:
* libGL.so.1
* libGLU.so.1
* libasound.so.2
* libusb-0.1.so.4
* libmad.so.0
* libmpeg2.so.0
* libmpeg2convert.so.0
* libstdc++.so.6
* libm.so.6
* libgcc_s.so.1
* libc.so.6
* libX11.so.6
* libpthread.so.0
* libdl.so.2
* librt.so.1
* libnvidia-tls.so.340.96
* libnvidia-glcore.so.340.96
* libXext.so.6
* libusb-1.0.so.0
* libxcb.so.1
* libudev.so.1
* libXau.so.6
* libXdmcp.so.6
* libcap.so.2
* libresolv.so.2

# Hook module configuration file
Checkout the usage information of the hook and set the option values according
to your needs. Here is an example option configuration file:
```
log_file_path=/tmp/pumptools.log
log_level=3
enable_file_monitor=0
enable_io_monitor=0
piuio_emu_lib_path=/pumptools/lib/piuio-emu.so
piuio_exit_test_service=1
game_settings_path=/save/fex
sound_device=hw:0
keyboard_dev=/dev/input/by-id/usb-Logitech_USB_Receiver-if02-event-mouse
halt_on_segv=0
```

# Run the game
Ensure you are running an X screen. Otherwise, you have to start one along with the game. Various library/system-calls
require root privileges. Make sure to run the game as root or with sudo (otherwise you get various sorts of errors,
typically permission denied). Use the included *run.sh* file to start the game on a desktop environment.

# Further notes
## Vsync
The game is required to run with vsync on.