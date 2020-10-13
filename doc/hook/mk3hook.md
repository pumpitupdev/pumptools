# mk3hook: 1st to Prex 3/Premiere 3 Linux ports
This readme covers any matters that are relevant for this hook, only. Anything that applies to **all** hooks is covered
in a [main hook readme file](../hook.md) including general data setup and a quick start guide.

First and foremost, the MK3 Linux ports are runnable without this hook as well. The ports were created from the original
fully disassembled DOS binaries and reassembled for 32-bit Linux. To make the games run on modern non-MK3 hardware,
additional features were added to the disassembly:
* Software lockchip emulation
* ISA PIUIO to USB PIUIO (over libusb)
* MP3 audio decoding and playback using fmodex
* Software sound effect playback
* Software EEPROM data reading/writing

However, since modifying the (decompiled) source code is a very complex task, various additional features and fixes
are applied using a hook library. This lacks certain flexibility but is so far sufficient to fix various bugs and add
some more quality of life features.

## Additional notable features
* Audio subsystem using fmodex: Bugfixes and audio device selection
* Relocate game data to `game` sub-folder to have identical data layout to newer generation games
* Additional development/debugging features, e.g. file, io, open logging and tracing
* Hardcoded OpenGL bugfix
* Utilize pumptools configuration infrastructure to unify configuration of all games

## Versions supported
Basically, "all" versions are supported considering there was only one Linux binary per game released so far.

## Quick start: how to run (official release), additional steps
Start with the quick start guide from the [main hook readme](../hook.md#quick-start-how-to-run-official-release) and
add the additional steps at the very end:
1. **1st and 2nd only**: Open the `hook.conf` file and set the following property: `game.1st_2nd_fs=1`. For details, see
[this section](#1st-or-2nd-errors-about-failed-resource-loading)
1. **1st only**: Go straight to the operator menu by pressing `G` on your keyboard. Go to `GAME OPTION`, select and
confirm `DEFAULT SETTING` and `SAVE AND EXIT`. Next, go to `COIN OPTION`, `DEFAULT SETTING` and `SAVE AND EXIT`. See
[this section](#1st-crashes-right-after-the-andamiro-logo-or-the-intro-sequence-when-i-should-see-the-title-screen-with-a-floating-point-exception)
for details.

## Dependencies
Make sure to read the different methods of dependency resolution available in the [main hook readme file](../hook.md),
first.

The following **direct** dependencies (cmd: `readelf -d piu`) are required by all Linux ports:
* libGL.so.1
* libX11.so.6
* libXxf86vm.so.1
* libXrandr.so.2
* libpthread.so.0
* libXi.so.6
* libXcursor.so.1
* libXinerama.so.1
* libm.so.6
* libncurses.so.5
* libtinfo.so.5
* libfmodex.so
* libconfig.so.9
* libusb-0.1.so.4
* libc.so.6

As for method 1, when using Ubuntu, the dependencies can be found in the following packages:
* libc-bin (or gcc-multilib on a 64-bit platform)
* libusb-0.1-4
* libconfig++9v5
* tinfo5
* ncurses5
* libxcursor1
* libxinerama1
* libxi6
* libxrandr2
* libxxf86vm1
* libx11-6
* libasound2

## Data setup
In additional to the [general information applying to **all** hooks](../hook.md#data-setup), the following information
goes for a clean set of data from a pristine/non-bootleg CD.

If you have the official release, this might not be relevant to you as it just explains some important technical details
about the data. 

In general, get a clean set of data from a pristine/non-bootleg CD.

However, 1st, 2nd and 3rd and Extra (IIRC) are the exception here. These games have their textures packed in an odd
format (IIRC something about an odd color format and/or textures being flipped) that cannot be used with the Linux port
versions. These versions require file modification and repacking to make everything look correct with the linux
binaries.

Furthermore, because the old games ran on case-insensitive file systems, everything goes with file naming depending on
the version of the game, e.g. full upper/lower-case files, folders and even mixed ones, e.g. Stage.cfg (*sigh*).
In order to make this less painful for the the hook library, some file names were modified to create some sort of
consistency.

You are not required to have the pulled data in the same location and layout as given by the original layout on the
disc as this can be configured with the hook library. Everything can be located locally in a single folder. The settings
for this can be found and tweaked in the `hook.conf` file which is created after you started the game once
([see below](#mk3hook-features)).

#### The game data subfolder
This differs slightly from newer generation games, i.e. MK6-based and newer. The hook library takes care of this by
implementing a consistent directory structure on all versions of the game. Therefore, all game data from the CD of MK3
games goes into a `game` folder as well. Example of `games` contents for Prex 3:
* `AUDIO`
* `BGA`
* `STEP`
* `TITLE`
* `PIU.BIN`
* `STAGE.CFG`

Furthermore, the game needs to have the sound effects available as files. These were stored on a ROM chip on the MK3
hardware. Luckily, newer PC-based games have these as normal files with the correct naming in the `game/WAVE` subfolder.
These also go into `game/EFFECTS`.

Some other and older versions come with different files and folders but the process is identical. The first three games,
1st, 2nd and 3rd, had their audio files stored on disc as PCM audio data next to the other game assets. These audio
files need to be ripped and provided as MP3 encoded audio files in the sub-folder `game/track`.

## Linux port features
This section covers the features that were added to the Linux ports and therefore not available on the original DOS
versions.

### Configuration file
A configuration file with different options can be provided to the game's bare executable. The configuration is provided
as a command line argument when running the game, e.g.
```
./piu ./save/config.cfg
```

Example contents:
```
fullscreen = 1;
allow_exit = 1;
save_file = "./save/EEPROM.BIN";
effect_path = "./EFFECTS/";
track_path = "./track/";
sync_offset = 115;
sync_multiplier = 4.16666666666666696272613990004;
music_volume = 1.0;
sfx_volume = 1.0;
```

Most entries are self-explanatory. However, there are two parameters that can be tweaked to control synchronisation of
the stepchart to the music.

The `sync_offset` parameter (in ms) is used to offset the stepchart to the music. However, as you might be used to such
a parameter from other music games, this offset is **NOT** entirely identical to adjusting the music to your sound
system's latency. You can use this value to shift the synchronisation point forward and backward but keep in mind that
it will not match any other offset values you determined in other games.

The `sync_multiplier` is also a variable that was depending on the sound hardware of the target platform. It was
already determined and there should be **NO** reason to change it unless you know what it does and how it is used in
synchronizing the stepchart to the song in the engine.

All these configuration values are exposed via the `hook.conf` file and hooked to the pumptools configuration
infrastructure. Usually, there is no need to deal with this directly, so this is section is mainly for documenting its
presence.

### Built-in keyboard controls 
Ignore this section if you are using the `mk3hook` because it will disable this feature entirely. See 
[this section](#configure-io).

The following keyboard controls are available with the Linux ports.

PIUIO hooked controls:
```
Test: Key G
Service: Key H
Clear: Key J
Coin 1: Key K
Coin 2: Key L

Pads (Player 2 on numpad):
Q   E  7   9
  S      5
Z   C  1   3
```

Keyboard controls supported by the game engine (won't work on very early versions of the game, e.g. 1st, 2nd, 3rd):
```
Test: F1
Service: F2
Clear: F3
```

### USB PIUIO
Ignore this section if you are using the `mk3hook` because it will disable this feature entirely. See 
[this section](#configure-io).

When a USB PIUIO is plugged it, the game will automatically detect and use it.

### Exit the game
Use `Test` + `Service` on the USB PIUIO (controls) to exit the game. This can also be blocked in the configuration.

## mk3hook features
Check the `hook.conf` file which is located in the same folder as the `piu` executable once you have started the game.
The available settings are explained in the `hook.conf` file.

### Configure IO
See [this section of the main hook readme](../hook.md#configure-io).

### Select another audio device
If the default sound card is not working, e.g. see [here](#no-sound-and-fmod-errors-in-log), you might want to
select another audio device/card instead.

In order to find out which cards are available and which configuration value to set, you have to run the game once.
Check the log and you will see a list of available devices printed somewhere at the beginning of the log, for example:
```
Output type: 11
Num available drivers 43
Driver 0: default
Driver 1: null
Driver 2: jack
Driver 3: sysdefault:CARD=PCH
Driver 4: front:CARD=PCH,DEV=0
...
```

By default and if you have not configured any sound device, yet, this will pick the first, usually, `default` device.
Pick another one by copying the name, e.g. `sysdefault:CARD=PCH`, and setting it in the `hook.conf` file:
```
patch.sound.device=sysdefault:CARD=PCH
```

When you restart the game, check the log if the device got picked up properly. This is indicated by a log message
right after the device list is printed.

### Debugging fmodex issues
You can also debug issues with fmodex, e.g. why 
[the selected sound device does not work](#no-sound-and-fmod-errors-in-log).

You need to replace the normal `fmodex.so` library in the game local `lib` folder with a `fmodexL.so` version. Just
rename the "L"-version to `fmodex.so` and have it in the game local `lib` folder. Furthermore, you have to enable
debug output by setting the following configuration value in the `hook.conf` file:
```
patch.sound.debug_output=0
```

After that's done, you should see additional log output by fmodex on the console.

## Troubleshooting and FAQ
Make sure to also check the
[troubleshooting and FAQ section of the main hook readme](../hook.md#troubleshooting-and-faq). This covers various
things that apply to **all** hooks. The following sub-sections apply mainly to this hook.

### No sound and FMOD errors in log
If you see one or multiple of the following error messages in the log output
```
FMOD error! (60) Error initializing output device. 
FMOD error! (79) This command failed because System::init or System::setDriver was not called. 
FMOD error! (37) An invalid parameter was passed to this function.
```
it is very likely that fmod cannot use the default/currently selected audio device for playback. This can have various
reasons from non suitable configuration to device being used by another process and therefore blocked if not supporting
software mixing.

Choose another device for playback instead, see [this section](#select-another-audio-device), or you can enable
fmodex debug output to further debug this issue, see [this section](#debugging-fmodex-issues).

### No sound and log is showing some error about audio group
If you see something like this in the log:
```text
Trapping getgrnam_r for audio group of libasound
audio group entry format in /etc/group invalid. Check your /etc/group file!
ALSA lib pcm_direct.c:2004:(snd1_pcm_direct_parse_open_conf) The field ipc_gid must be a valid group (create group audio)
```

You have to setup the audio group properly. Usually the following command gets the job done for this:
```bash
sudo usermod -a -G audio piu
```

Replace `piu` with the username that you are using to run the game (typically user with id 1000). If everything's fine,
you should see the following output (or similar) instead of the error above:
```text
Trapping getgrnam_r for audio group of libasound
Resulting group struct: audio x 995 piu,
```

### 1st/2nd is missing the "Insert Coin" text on the title screen and several other graphics, e.g. life bar during gameplay
This is a known bug and we do not have a fix for this so far. However, this is not always triggered and if you keep
restarting the game (with a few seconds of waiting between restarts), you will start the game without this bug being
active eventually.

### 1st or 2nd errors about failed resource loading
For example:
```
FAIL: res_load( piu.dat )
```

This error indicates that one or multiple game files are not placed in the right location(s), your configured `game`
sub-directory path is not correct or you forgot to switch on the "1st/2nd filesystem" feature switch in the hook
configuration. For the latter, set the following in your `hook.conf` file:
```
game.1st_2nd_fs=1
```

### 1st crashes right after the Andamiro logo or the intro sequence when I should see the title screen with a floating point exception
1st does not detect if no EEPROM data is available and does not write defaults on first start or when you deleted the 
`save/EEPROM.BIN` file. Therefore, all settings values for the game are considered "0". This is fine with all settings
except the coin settings. This causes the floating point exception though I assume it was actually caused by a division
by zero.

To fix this, go right to the operator menu when you booted up the game and are still on the Andamiro logo screen. Go
to `GAME OPTION`, select and confirm `DEFAULT SETTING` and `SAVE AND EXIT`. Next, go to `COIN OPTION`, `DEFAULT SETTING`
and `SAVE AND EXIT`. This writes a fresh `EEPROM.bin` file with all default settings and you are good to go.