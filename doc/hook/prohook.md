# prohook: Pro (1)
This readme covers any matters that are relevant for this hook, only. Anything that applies to **all** hooks is covered
in a [main hook readme file](../hook.md) including general data setup and a quick start guide.

## Versions supported
All known versions supported if no-dongle patched at this time.

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

## Data setup
A clean set of data won't work here. You need the decrypted data zip files and a patched executable that uses plain zip
files instead of encrypted ones.

All access to files and folders are detoured to two folders which makes setting up everything easier.

You need two main folders:
* `game` folder with the following contents
    * All data zip files (e.g. `data0.zip`, `data1.zip`, ..., `encore.zip`)
    * An empty file `FX` (if you run this on a FX or other cabinet with a widescreen monitor)
* `save ` folder. Contents are generated automatically if empty.

## USB thumb drive/profile support
Without any patches, the game's way of handling USB thumb drives is incompatible to the kernels of the last years.
This is fixed by prohook but requires some additional configuration effort.

1. Decide which physical USB ports of your machine you are going to use for the player 1 and player 2 sides. 
1. Plug in *one* USB thumb drive into the player 1 port. Now you have to figure out two things:
    1. The device node the drive got assigned to by the kernel, e.g. `sdb`. For example, use the command `fdisk` or
    `lsblk` for this and take a note.
    1. The USB bus and port number the thumb drive is plugged into. `ls -la /sys/block` shows you all currently
    available block devices, including USB thumb drives. Find the thumb drive you plugged into the physical port and
    check the path the symlink gets resolved to. For example: 
    `sdc -> ../devices/pci0000:00/0000:00:14.0/usb2/2-1/2-1:1.0/host7/target7:0:0/7:0:0:0/block/sdd`
    You can identify the bus and port by taking a look at the part after the `usb2` directory here: `2-2`. The format
    is `bus-port` which means that in this example the drive is plugged into port 1 of bus 2. Take a note of that.
1. Repeat the previous step for the physical port you consider using for player 2.
1. Create two folders that will serve as mount points for the two player sides in the `/mnt` directory, e.g. 
`mkdir /mnt/0 && mkdir /mnt/1` (or re-using these folders if they already exist from other piu games).
1. Add the following entry to the `/etc/fstab` file on the machine you want to run the game on:
```text
/dev/sdb1	/mnt/0	auto	noauto,owner	0	0
/dev/sdc1	/mnt/1	auto	noauto,owner	0	0
```
Note: This does not create a fixed assignment of `/dev/sdb1` or `/dev/sdc1` to a specific physical USB port. It just
ensures that any block device that gets enumerated as `/dev/sdb` will get its first partition mounted to `/mnt/0`
(same concept applies to `/dev/sdc1/`).
1. Modify the `hook.conf` file used with `prohook.so` by inserting the device nodes and bus-port combinations into the
respective fields, for example:
```text
patch.usb_profile.p1.bus_port=2-1
patch.usb_profile.p2.bus_port=2-2
# Note: No partitions, e.g. sdb1, here
patch.usb_profile.dev_nodes=sdb,sdc
```

Once you plug-in a USB thumb drive to the configured ports, it should show up on the correct player side.

## Troubleshooting and FAQ
Make sure to also check the
[troubleshooting and FAQ section of the main hook readme](../hook.md#troubleshooting-and-faq). This covers various
things that apply to **all** hooks. The following sub-sections apply mainly to this hook.

## Enable log output from the game
If you run into any issues, consider enabling the game's log output to console which might help you figuring out
what's going on. This can be done by editing the `data.Static.ini` file in the `data0.zip` file. Go to the section
`[Options-arcade]` and change the property `ShowLogOutput=0` to `ShowLogOutput=1`.

## Switch screen aspect ratio: 4:3 and 16:9
The game supports 4:3 and 16:9 aspect ratios. To enable 16:9 mode, ensure that an empty file called `FX` is placed
next to the data zip files inside the `game` directory. For 4:3 mode, simply delete the `FX` file if it exists.

## ITG2 PIUIO kernel module hack
If you don't have the original kernel module installed and you are running a real PIUIO, you have to hook the
`ptapi-io-piuio-real.so` lib which implements the piuio api. The "emulation" part which simply calls back to a real
piuio driver takes care of handling the kernel hack path then. If you run on a real IO without hooking that module,
you won't get any inputs or outputs.

## The game is crashing very early
The log might show something about "ptrace failing" and not being able to "load libformat". However, the actual problem
is probably not having all the necessary IO hardware connected (or emulation enabled). The game doesn't have any proper
error handling if either the PIUIO or the button IO is missing/not connected (or not emulated).