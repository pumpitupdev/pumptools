# x2hook: Exceed 2
This readme covers any matters that are relevant for this hook, only. Anything that applies to **all** hooks is covered
in a [main hook readme file](../hook.md) including general data setup and a quick start guide.

## Versions supported
* 102

Any other version won't work, period. The hook has to memory patch a weird bug causing the sound thread to crash.

## Dependencies
Make sure to read the different methods of dependency resolution available in the [main hook readme file](../hook.md),
first.

Note: Game is 32-bit, so you need to install the 32-bit versions of the dependencies!

The following **direct** dependencies (cmd: `readelf -d piu`) are required:
* libGL.so.1
* libGLU.so.1
* libpthread.so.0
* libmad.so.0
* libasound.so.2
* libz.so.1
* libpng12.so.0
* libmpeg2.so.0
* libmpeg2convert.so.0
* libusb-0.1.so.4
* liblua50.so.5.0
* liblualib50.so.5.0
* libgcc_s.so.1
* libc.so.6
* libm.so.6
* libX11.so.6
* libstdc++.so.5

As for method 1, when using Ubuntu, the dependencies can be found in the following packages:
* libc-bin (or gcc-multilib on a 64-bit platform)
* libx11-6
* zlib1g
* libusb-0.1-4
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

### Fully unlock the game
This hook has an option to fully unlock the game (well, unlock Canon-D Full Mix without entering the code because that's
the only permanent unlock) by setting the following property in the configuration file:
```text
game.force_unlock=1
```

### Using the unlock option to unlock Canon-D full remix does not work when enabled
When the game cannot read from an existing `PIUEXCEED.INI` file, the settings are created in-memory and written to disk.
Therefore, the unlock mechanism of the hook cannot apply this to the contents in-memory but only when the
`PIUEXCEED.INI` file is re-loaded. To achieve this, once you started the game to create a new `PIUEXCEED.INI` with
default values, simply quit the game and restart.