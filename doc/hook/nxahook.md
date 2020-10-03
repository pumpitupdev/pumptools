# nxahook: NXA
This readme covers any matters that are relevant for this hook, only. Anything that applies to **all** hooks is covered
in a [main hook readme file](../hook.md) including general data setup and a quick start guide.

## Additional notable features
* Removed USB flash drive vendor lock, i.e. use _ANY_ USB flash drive to store game profiles
* Auto generate new profiles if no profile is found on the connected USB flash drive

# Versions supported
All known versions supported.

## Dependencies
Make sure to read the different methods of dependency resolution available in the [main hook readme file](../hook.md),
first.

The following **direct** dependencies (cmd: `readelf -d piu`) are required:
* libfreetype.so.6
* librt.so.1
* libGL.so.1
* libGLU.so.1
* libusb-0.1.so.4
* libpthread.so.0
* libXxf86vm.so.1
* libpng12.so.0
* libasound.so.2
* libmad.so.0
* libboost_regex-mt.so.3
* libgcc_s.so.1
* libc.so.6
* libm.so.6
* libX11.so.6
* libdl.so.2
* libstdc++.so.6
* libz.so.1

As for method 1, when using Ubuntu, the dependencies can be found in the following packages:
* libc-bin (or gcc-multilib on a 64-bit platform)
* libx11-6
* zlib1g
* libusb-0.1-4
* libasound2

## Data setup
In additional to the [general information applying to **all** hooks](../hook.md#data-setup), this game requires all
files and folders from the original `game` folder to be in **UPPERCASE** on a case-sensitive file system. Further game
asset files and folders from cramfs need to be copied to the `game` directory. `nx.ttf`, `nxcn.ttf`, `nxpt.ttf`,
`nxtw.ttf`, `mission.txt` and `ufo.txt` must be **lowercase** but `SCRIPT` and its contents must be **UPPERCASE**.

The `config` (or `CONFIG`) folder and its contents must be available in **UPPER** _AND_ **lowercase**. The `BrainQuest`
folder name must be kept like this and its contents must be **lowercase**.

## Troubleshooting and FAQ
Make sure to also check the
[troubleshooting and FAQ section of the main hook readme](../hook.md#troubleshooting-and-faq). This covers various
things that apply to **all** hooks. The following sub-sections apply mainly to this hook.