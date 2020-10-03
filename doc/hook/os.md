# Hardware
Everything runs on original hardware MK6 and up. However, newer games might be slightly more demanding and will not
run smoothly on the old MK6 anymore.

As for custom hardware, everything that's at least of MK6 specs should do the job. GPU-wise you are not bound to nVidia
cards. Though, the card of your choice has to support at least the OpenGL 2.0 standard.

However, not that we have seen graphical glitches on AMD and Intel GPUs with NX2 which we haven't seen on older pump
versions. The game starts and runs but after some time you might encounter graphical glitches.

You can also run the games in a VM. Recommended settings: 2 vCPUs, 2 GB RAM, OpenGL 2.0 hardware acceleration enabled
(requires installing drivers/kernel modules on your guest).

# Operating system and environment setup
This document outlines common stuff that is required to run any of the games supported by pumptools. Make sure to follow
these properly and have your system prepared for the hooks before you continue with their dedicated readme files.

## Linux distributions supported
If you have the required knowledge, you should be able to get this to run on any distribution available. However, there
are a few things to consider:
* The game is a 32-bit binary, you need to be able to run 32-bit binaries
* Naturally, the game requires 32-bit versions of the libraries it depends on
* The game is built with glibc. You might have a hard time on distributions not shipping with them by default, e.g.
Alpine Linux
* Kernel-wise, the latest kernels and everything that's not pre 2.6 should work. Due to various hook sub-modules, many
quirks in different games have been fixed to run all games on the latest upstream kernel versions of Linux.

Recommendation: Use a 64-bit Linux distribution of your choice. You can also use a 32-bit distribution as this is the
arch these games were built for but it does not come with any benefits. Prepare yourself to upgrade to 64-bit once any
of the newer games (finally) moves to that target architecture.

## Dependencies
Each game and hook comes with a list of dependencies you might have to install. This depends on which type of dependency
resolution you pick.

When running this on a 64-bit Ubuntu, all packages must be installed as lib32 packages! When you are on Debian/Ubuntu
(based) distros and you are using apt, you have to enable multiarch support first:
```shell script
sudo dpkg --add-architecture i386
apt update
```

Then, install the 32-bit packages like this where `<pkgname>` must be replaced accordingly:
```shell script
apt install pkgname:i386
```

Note: The `:386` postfix does not apply to packages like `gccmultilib` as this already covers the i386 part.

## GPU drivers
For `libGL`, you need to install a GPU driver and have hardware acceleration enabled. The `libGL` library provided also
needs to be 32-bit compatible.

To check if everything's installed and HW acceleration is working, just run `glxgears`. If that doesn't error and you
see the rendered gears spinning, you should be fine.

## Sound drivers
Naturally, you also have to have sound stuff installed. Having alsa with its libraries available is sufficient. Pulse
and similar audio systems are not required, e.g. when running on a cabinet.