# exchook: Exceed
This readme covers any matters that are relevant for this hook, only. Anything that applies to **all** hooks is covered
in a [main hook readme file](../hook.md) including general data setup and a quick start guide.

## Additional notable features
* Full MK5IO emulation with API hook: Keyboard, MK6 PIUIO or your own custom IO
* Fixed infamous hold glitch, i.e. make the damn game playable, finally

## Versions supported
* 20040325
* 20040408

Any other version won't work, period. Pumptools has to memory patch various things like I/O because hooking them is not
possible (at least not right now).

## Dependencies
Make sure to read the different methods of dependency resolution available in the [main hook readme file](../hook.md),
first.

Note: Game is 32-bit, so you need to install the 32-bit versions of the dependencies!

The following **direct** dependencies (cmd: `readelf -d piu`) are required:
* libGL.so.1
* libGLU.so.1
* libpthread.so.0
* libasound.so.2
* libz.so.1
* libpng12.so.0
* libc.so.6
* libm.so.6
* libX11.so.6
* libstdc++.so.5

Additionally, when using `piuio.so`, you need the following library as well:
* libusb-1.0.so.0

As for method 1, when using Ubuntu, the dependencies can be found in the following packages:
* libc-bin (or gcc-multilib on a 64-bit platform)
* libusb-1.0-0
* libx11-6
* zlib1g
* libasound2

Further indirect dependencies are needed but should be taken care of automatically when using a package manager to
install the direct dependencies.

## Data setup
In additional to the [general information applying to **all** hooks](../hook.md#data-setup), this game requires all
files and folders in the `game` folder to be in **UPPERCASE** on a case-sensitive file system.

## Troubleshooting and FAQ
Make sure to also check the
[troubleshooting and FAQ section of the main hook readme](../hook.md#troubleshooting-and-faq). This covers various
things that apply to **all** hooks. The following sub-sections apply mainly to this hook.

### The game crashes when using the sound device hw:0
The game's sound manager relies on hardware mixing using the same device which is not supported by newer versions of
alsa anymore. Instead, you have to use software mixing using the `dmix` device,
[see here](#the-game's-music-plays-too-fast-or-sounds-weird).