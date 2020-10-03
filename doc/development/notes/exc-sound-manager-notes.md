# Notes about Exceed's implementation of a sound manager
## Pre-face
To understand what the developers of AM did here, you should have some knowledge about how they had to handle sound on
the MK3 hardware which is the predecessor of the MK5 hardware that was used with Exceed. Exceed builds on the codebase
of the previous game(s), Prex 3 and Premiere 3, that ran on MK3/MK5. However, moving from Dos (MK3)/Windows XP Embedded
(MK5) to Linux (MK5), the code had to be adjusted to work on the different kind of hardware and operating system.
Their engine was targeted and optimized for the MK3 hardware platform and had to use various pieces of custom hardware
to implement stuff like: security using the lock chip, MP3 decoding and effect playback and IO.

## Sound playback, higher level view
For sound playback, the game had the following requirements:
1. For playing a song, menu background music, title screen intro music or any other music tracks, sound data encoded as
MP3 files are used. These needed to be loaded, decoded and played back whenever the game reaches certain points in the
game scene.
1. Various short sound effects, e.g. to give the user confirmation about hitting a button, have to be played back when
a certain event is triggered, e.g. user input or a scripted event in a scene. On exceed, these are stored as WAV files.
1. Audio played back by the above two items needs to be mixed because playback of both can happen simultaneously.

## MK3 hardware for decoding and audio playback
The MK3 hardware came with a dedicated piece of hardware, a [MP3 decoder and playback chip](hardware.md). Therefore,
audio data from disc was streamed encoded to the hardware for decoding and playback, and effects were triggered by
commands to the appropriate chips as their audio data is stored in a ROM chip on the board.

## Exceed's sound manager
With the move to Linux and less specialized hardware closer to a normal PC, the audio pipeline changed a lot. On Linux,
alsa/libasound is used for sending data to the underlying audio driver. Decoding of the MP3 song data is done by the
CPU. Effects are stored as WAV files and also played back using libasound.

The sound manager relies on hardware mixing using the same sound device, e.g. hw:0. This device is opened once on game
init for playing back sound effects and another time for every song/background music to be played back for the current
scene.

### Hardware sound tracks specs
#### Effect track
* Channels: 2
* Buffer size in frames: 4096
* Periods: 2
* Sample format: 16-bit signed LE
* Sample rate: 44100
* Sample order: FL, FR

#### Song track
* Channels: 2
* Buffer size in frames: 16384
* Periods: 8
* Sample format: 16-bit signed LE
* Sample rate: 44100
* Sample order: FL, FR

### Issues on todays distros
However, this way of mixing is not supported anymore by alsa. Instead, you have to use software mixing using dmix which
is supported by alsa's built in plugin system today. But, that doesn't seem to work properly with different
configurations from different versions of alsa on today's available distros.

Fortunately, this got fixed already on Exceed 2 which implements its own software mixer in the pump engine.