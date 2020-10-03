# Notable features

* Runs on recent kernel versions
* Runs on 32-bit and 64-bit distros (64-bit distros require additional 32-bit libs to be installed)
* Dongle stuff must be patched out
* You need decrypted data zips to run this
* Full MK6IO emulation with API hook: Keyboard or your own custom IO
* Full PIUBTN emulation with API hook: Keyboard or your own custom IO
* Real IO passthrough (for MK6 usb io and PIUBTN)

# Versions supported
Currently, only the latest revision is supported (I think that's R5?)

# Data setup
A clean set of data won't work here. Glenn protected the data with his own life
(pretty much) and made it as obnoxious as possible to unpack it. So good luck
getting the original data out of a drive image (if you didn't get it already
from somewhere).

All access to files and folders are detoured to two folders which makes setting
up everything easier.

You need one main folder:
* game/pro2 (contains all the game data, stepmania layout)

## Executable dependencies
All dependencies must be compiled as 32-bit binaries. Here is a list of
dependencies (with versions) required to run the game:
* libXtst.so.6
* libXrandr.so.2
* libGL.so.1
* libGLU.so.1
* libdl.so.2
* libavformat.so.51
* libavcodec.so.51
* libavutil.so.49
* libusb-0.1.so.4
* libpthread.so.0
* librt.so.1
* libstdc++.so.5
* libm.so.6
* libgcc_s.so.1
* libc.so.6
* libX11.so.6
* libXext.so.6
* libXi.so.6
* libXrender.so.1
* libnvidia-tls.so.340.106
* libnvidia-glcore.so.340.106
* libstdc++.so.6
* libxcb.so.1
* libXau.so.6
* libXdmcp.so.6

# Hook module configuration file
Checkout the usage information of the hook and set the option values according
to your needs. Here is an example option configuration file:
```
log_file_path=./pumptools.log
log_level=3
enable_file_monitor=0
enable_io_monitor=0
piubtn_emu_lib_path=
piubtn_real_passthrough=1
piuio_emu_lib_path=./piuio.so
piuio_real_passthrough=1
piuio_exit_test_service=1
game_data_path=./game
```

# Run the game
Ensure you are running an X screen. Run the game using
Ensure you are running an X screen. Otherwise, you have to start one along with the game. Various library/system-calls
require root privileges. Make sure to run the game as root or with sudo (otherwise you get various sorts of errors,
typically permission denied). Use the included *run.sh* file to start the game on a desktop environment.

# Further notes
## ITG2 PIUIO kernel module hack
If you don't have the original kernel module installed and you are running a
real PIUIO, you have to hook the *piuio.so* lib which implements the piuio api.
The "emulation" part which simply calls back to a real piuio driver takes
care of handling the kernel hack path then. If you run on a real IO without
hooking that module, you won't get an inputs or outputs.

## The game is crashing very early
This is probably due to not having the right IO hardware connected/emulated.
The game doesn't have any proper error handling if either the PIUIO or the
button IO is missing/not connected (or not emulated).

## Vsync
The game is required to run with vsync on.