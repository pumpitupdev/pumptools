# pro2hook: Pro 2
This readme covers any matters that are relevant for this hook, only. Anything that applies to **all** hooks is covered
in a [main hook readme file](../hook.md) including general data setup and a quick start guide.

## Versions supported
All known versions supported as long as it uses fully unpacked assets and a nodongle executable.

## Dependencies
Make sure to read the different methods of dependency resolution available in the [main hook readme file](../hook.md),
first.

The following **direct** dependencies (cmd: `readelf -d piu`) are required:
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

As for method 1, when using Ubuntu, the dependencies can be found in the following packages:
* libc-bin (or gcc-multilib on a 64-bit platform)
* libx11-6
* libusb-0.1-4
* libasound2
* ffmpeg

## Data setup
A clean set of data won't work here. You need the dumped and unpacked/decrypted data and a patched
executable that runs either on plain zip or files.

All access to files and folders are detoured to one folder, the `game` folder, which makes setting
up everything easier. The `game` folder contains the typical StepMania folders:
* BGAnimations
* BackgroundEffects
* BackgroundTransitions
* Characters
* Courses
* Data
* Logs
* NoteSkins
* RandomMovies
* Save
* SongMovies
* Songs
* Themes

Each folder with the content from the dump.

## USB thumb drive/profile support
See instructions on the [prohook readme](prohook.md#usb-thumb-drive-profile-support).

## Troubleshooting and FAQ
Since the game is based on Pro 1, have a look at the
[troubleshooting section there](19-pro.md#troubleshooting-and-faq). Most of the things listed there
apply to Pro 2 as well (or are very similar). Further Pro 2 exclusive items are listed in this
section.
