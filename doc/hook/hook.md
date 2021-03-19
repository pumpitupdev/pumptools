# Pumptool's hook libraries
A collection of libraries that need to be pre-loaded when running vanilla dumps of Pump It Up games. These hooks allow
you to run any of the supported games on any* Linux distribution and hardware.

Each game might require a different hook library as the software evolved as well as the hardware and original
operating system changed a few times as well.

## General features
A few notable features:

* Run any supported gam on recent kernel versions thanks to various fixes
* Run any supported game on 32-bit and 64-bit distros (64-bit distros require additional 32-bit libs to be installed)
* Full dongle emulation
* Remove HDD checks to run this on "non legit" drives
* Full IO hardware emulation: MK6 PIUIO, Pro Button board (PIUBTN)
* Real IO passthrough
* API: Implement support for your own custom IO

## Supported games and versions
Check each of the dedicated hook readmes which games and versions are supported.

## Dependencies of hook libraries
Pumptool's hook libraries aim for having no dependencies other than what is already required by the
different games to run.

Since all games available so far are compiled as 32-bit binaries, naturally, it requires you to install
the 32-bit versions of the dependencies (if you are not running a 32-bit distributed).
Note the `:i386` postfix for packages in the command further below. This is required if you are on a 64-bit
distribution.

However, there are some exceptions that require additional libraries in order to allow the following
features to work correctly:
* Hook libraries, e.g. `nx2hook` that support network features, e.g. usb profiles with pumpnet
* IO API implementations that support real devices, e.g. real PIUIO, which use libusb-1.0

Taken from [the Dockerfile for building pumptools](../../Dockerfile), the following commands install
the dependencies that you need on an Ubuntu-based system:

```bash
dpkg --add-architecture i386
apt-get update && apt-get install -y \
    g++-multilib \
    gcc-multilib \
    libc6-dev-i386 \
    libusb-1.0-0:i386 \
    libusb:i386 \
    libasound2:i386 \
    libconfig++:i386 \
    libx11:i386 \
    libcurl4-gnutls:i386
```

Further game version specific dependencies and how to set these up is outlined in
[its own section](#dependencies-of-games).

## Hardware, operating system and environment
A general outline is given by [this readme](os.md) if you want to setup something yourself. Otherwise, you should
checkout the `pumpos` project in a repository nearby which takes care of installing a fully configured OS to a physical
disk to run the games on dedicated hardware for cabinets.

## Quick start: how to run (official release)
The following steps apply to any game of the "officially" supported release data.

1. Install the required dependencies which can vary per game. Check the section "required dependencies" in the dedicated
readme files of each hook.
1. Unpack `game.zip` and `lib-local.zip` to a folder of your choice.
1. Your folder should contain the following files and folders: `game`, `lib`, `piu`, `version`
1. Unpack the hook which supports the game you have chosen, e.g. for Exceed use `exchook.zip`, from the
`pumptools-X.XX.zip` release package next to the `piu` executable
1. Unpack the `piuio.zip` from the `pumptools-X.XX.zip` release package next to the `piu` executable
1. Rename the hook library, e.g. for Exceed `exchook.so`, to `hook.so` and the hook configuration file, e.g. for
Exceed `exchook.conf`, to `hook.conf`
1. Open `hook.conf` with a text editor and set the `patch.piuio.emu_lib` property accordingly:
    * For keyboard usage: `patch.piuio.emu_lib=./ptapi-io-piuio-keyboard.so` and
    `patch_hook_main_loop.x11_input_handler=./ptapi-io-piuio-keyboard.so"`
        * Configure your keyboard mappings using `./ptapi-io-piuio-keyboard-conf`
    * For USB PIUIO usage: Do not set the `patch.piuio.emu_lib` property and have the hardware plugged in.
1. Run the game as logged in root user: `./piueb run` or if you have `sudo` installed and configured: `sudo ./piueb run`

Details to specific games are given in the hook read files dedicated to each supported version. Further general
configuration and technical details as well as troubleshooting known issues are described in following sections.

## Dependencies of games
A list of dependencies is provided in the dedicated hook readme files for each game. The following is a general guide
on how dependencies of the games can be resolved to run them.

Right now, there are two methods for resolving the dependencies and which dependencies to use for the game:
* __Method 1__: Install as many dependencies as possible using the package manager of your distribution. Usually, you want
to go for method 1 and if the game runs, you don't have to bother with method 2. A few less common libraries are
provided with the official data release and are loaded from the local `lib` folder instead.
* __Method 2__: Provide **all** libraries except GPU related ones and a dedicated ld-linux loader with the game
independent from your system. Theoretically, this gives you full distribution independence but it is more complicated
and comes with a few unresolved issues so far. If method 1 doesn't work for you, try this method. For details, see the
[following section](#local-data-folder).

## Data setup
You are expected to get a clean set of data from a pristine drive. Ensure that the game is supported by one of the
hook libraries coming with pumptools and the game's version is on the list supported versions.

You are not required to have the pulled data in the same locations as on the original drive as the hook library can
be configured to have everything in a single local folder. These settings can be found and tweaked in the `hook.conf`
file which is created after you started the game once.

### Local data folder
Your local data folder must contain the following folders and files:
* `game`: The `game` asset folder from the HDD. Filename casing depends on the games and is relevant on case-sensitive
file systems. Exact requirements are explained in the readme files of each hook. Otherwise, the game crashes because of
files/folders it cannot find. Furthermore, put any additional files/folders that are game assets and not located in
the original `game` folder into the `game` folder, e.g. `mission.txt`, `SCRIPT` folder etc. which are located in
the cramfs on some games.
* `lib`: Put any libraries (especially older versions of libraries that can't be installed anymore using the package
manager) the game uses and aren't installed on your system in here. Using piueb, you have two options with potential
different (in-)compatibility issues:
    1. Have **all** libraries the game requires to run (except GPU driver specific libs) with compatible version in that
       folder including a dedicated `ld-linux.so`. See [piueb script header documentation](../../dist/piueb).
    1. Have only additional libraries that are not common/available on with your package manager **without** a dedicated 
       `ld-linux.so`.
* `save`: Empty folder where the the game stores configuration. These files are created by the game automatically and
contain default values if missing.
* `piu`: The Linux port `piu` executable.

## Configure IO
The hooks allow you to hook any implementation of [pumptools's API](../api/api.md) to the game to drive any type of IO
hardware. See the [dedicated readme](../api/io/piuio.md) on how to configure the PIUIO with the different types
of implementations available, e.g. keyboard, joystick, ...

## Troubleshooting and FAQ
The following sub-sections apply to all hooks.

### USB 3.0 vs 2.0 issues with USB thumb drives/profiles
This affects *ALL* games that make use of USB thumb drives for storing player profile related data.

Due to how the Linux kernel treats bus-port mappings for USB 3.0 and USB 2.0 different, it is recommended to limit the
usage of USB thumb drives either to 3.0 or 2.0 drives only after having configured the port assignment for the affected
games.

For example, you use a USB 2.0 thumb drive to assign one physical USB ports on your machine to each player side using
NX2's operator menu configuration option. However, the configured assignment is only working for USB 2.0 drives. If
you repeat this configuration step with a USB 3.0 drive (if your mainboard supports them because it has a USB 3.0
host controller), you will get different `bus:port` values shown on the configuration screen. Keep this in mind when
setting up the game, assigning the ports and using USB thumb drives with the games.

### My USB thumb drive is not detected by the game at all
Which means you cannot use it to even map the USB ports in the test menu.

Make sure to try at least another one by a different brand. There have been reports of some thumb drives simply not
working, e.g. a fairly old Kingston 1GB. In general, everything that gets detected fine by Linux should work.

### My USB thumb drive works when mapping the ports in the test menu but is not recognized on the game login screen
Might be the same issue as [here](#my-usb-thumb-drive-is-not-detected-by-the-game-at-all). Try out different USB thumb
drives. Be aware of the [USB 3.0 vs 2.0 issue](#usb-30-vs-20-issues-with-usb-thumb-drivesprofiles) as well.

### The game enters the operator menu when I start it
This is known to happen consistently on NX but was observed on other versions like Exceed 2 and Zero in the past. The
cause for this is unknown so far. Even with all usb emulation layers removed from pumptools and a real MK6 PIUIO
attached this still happens. If the game is run without the PIUIO attached, everything's fine. Therefore, we suspect
this is a bug within the game's PIUIO driver, likely some uninitialized buffers (due to the randomness of this bug
being triggered).

### How to I generate a fresh configuration file with default values
If you deleted your `hook.conf` file or you just want a clean start with default values, the hook creates a clean
`hook.conf` file if none exists once you run `piueb`.

### What are the command line arguments supported by the hook
Just run `piueb run -h` to get help output from the hook library. This also provides you with shortened command line
parameters for all options available from the configuration file.

### How do I provide command line arguments to quickly change configuration settings for the game
Just run `piueb run` with the shortened command line parameter, e.g. `piueb run -w`.

### The game's music plays too fast, too slow, or sounds weird
Depending on the game version you play, the audio subsystem is set to either render to an output device with a
frequency of 44100hz or 48000hz in SE16_LE format. Alsa needs to be configured accordingly to play back the audio
data at the right sample rate to make it sound right.

Games and audio settings required:
* 44100hz: MK3 Linux ports 1st to Prex 3, Exceed (1), Pro 1 and Pro 2
* 48000hz: Exceed 2 and newer

One possibility to fix that is change the values in your config, e.g. `/usr/share/alsa/pcm/dmix.conf` when using the
`dmix` device (the location and config can differ depending on your setup). In case you have to set them to 48000hz, 
search for the following configuration values and replace them with these values:
```
format S16_LE
rate 48000
```

If the contents of the files are just variables, check `/usr/share/alsa/alsa.conf` something similar like this:
```text
defaults.pcm.dmix.rate 44100
defaults.pcm.dmix.format "S16_LE"
```

The same method applies to replacing 44100hz with 48000hz.

### How do I figure out which sound device to select
You can list the currently connected devices/sound cards using the following command:
```shell script
cat /proc/asound/cards
```

Example output:
```text
 0 [PCH            ]: HDA-Intel - HDA Intel PCH
                      HDA Intel PCH at 0xfb610000 irq 51
 1 [NVidia         ]: HDA-Intel - HDA NVidia
                      HDA NVidia at 0xfb080000 irq 52
```

You can see two audio devices available: `0` being the built-in sound chip and `1` the audio output on the installed
GPU (i.e. HDMI audio out).

To route the audio to the device of your choice, e.g. device `0`, add the number to the `hw:` path: `hw:0` for device
`0`. Set `hw:0` in the configuration file:

```text
patch.sound.device=hw:0
```

### The game plays/renders too fast
The game relies on vsync to lock to the target framerate of 60 FPS. Ensure vsync is turned on in your GPU settings.

### libGL.so.1: cannot open shared object file: No such file or directory
Install your GPU drivers. This library depends on the GPU driver and is not included with the distributed data.

### There's no sound at all, even with the correct sound device selected
On certain setups, the sound output only seems to work after already having attempted to use the sound device once.
In a shell, try running the following command twice:

```shell script
aplay -q /usr/share/sounds/alsa/Front_Center.wav
```

If after a fresh boot the sound plays after the second attempt, your setup suffers from this issue.
Simply putting the above command once in a boot script will make sure the sound device is activated.

### How do I enable fullscreen/window mode?
It depends on the game. All MK3 linux ports have a window/fullscreen option that actually works. For games of the PRO
series, you have to [tweak game specific configuration files](prohook.md#windowfullscreen-mode). All other classic PIU
games depend on how you have your current Linux environment/X-server set, up. On a desktop environment, these games will
always start in window mode.

In general, there is no actual full-screen option on Linux like you have on Windows with DirectX. Fullscreen on Linux
means that you switch resolution of whatever X session you have currently running and fill the screen with the view
of your application.

The best you can do to go fullscreen on desktop is exactly the above. This can be achieved like follows:

```shell script
sudo xrandr --output HDMI-0 --mode 640x480
sudo ./piueb run
# Or whatever is your default resolution on your monitor
sudo xrandr --output HDMI-0 --mode 1024x768
```

Otherwise, it is recommended to setup a dedicated x-session using `startx`, e.g. `startx ./piueb run` and just have
only the game run in that session. A good reference for this might be the various scripts included in
[pumpos](https://dev.s-ul.net/hackitup/pumpos/-/tree/master/dist/piu/base/pumpos/data/00_bootstrap).

If you have a multi-monitor setup, things are getting quite complicated. Therefore, we are not covering this in here.

Also note that if you want the game to render at a higher resolution than its defined one, e.g. 640x480, this is not
possible right now. The output you get from the frame buffer will always be its native resolution.

### The game window is small and the 4:3 games are stretched when full screen on modern widescreen displays
On supported games, you can change the option `gfx.scaling_mode` in the `hook.conf` file to enable different pre-defined
scaling modes. Especially for the SD 640x480 4:3 games, this allows you to upscale the rendered output to a widescreen
display resolution while keeping the aspect ratio (pillarbox).

Note: This does not change the enhance the image quality. It just uses the GPU to interpolate the lower resolution
output and stretches it to the target surface area.