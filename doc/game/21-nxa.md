# Pump it Up NX Absolute

This document collects various information about the game itself.

## Trivia

* Release: December 2008
* USB player profiles from NX2 can be carried over and are extended by the game on first load. NXA
USB profiles are not backwards compatible with NX2.
* The original game only allows special brand of usb thumb drives. Other drives are not accepted by
the game. Profiles were managed and uploaded by the player using a proprietary software called
"PUMBI" which is no longer supported.

# Game modes

* Arcade Station: Separated into 3 channels. Each channel contains songs with chart difficulties Normal, Hard, Crazy,
Freestyle, Nightmare
  * New Tunes
  * Banya
  * K-Pop
  * Pop
* Training Station
* Brain Shower: Solve puzzles/quiz questions while playing a song
* Special Station: Full versions, remixes and another step (charts)
* World Max: Clear missions to unlock further locked missions and content in Special Station

## World max guide and unlocks

Either take a look at the `missions.txt` file which is either located on the compressed file system
of an original drive or in the `game` data folder of some data release, or use
[blood618's guide](guide/nxa.pdf) which was published on ph-online back in the days. 

### Missions.txt

Take a look at the `missions.txt` file which should easily reveal a bunch of useful information
already like mission conditions.

### blood618's guide

See [here](guide/nxa.pdf).

### Unlocks sourced from disassembly (icex2)

Sourced from the disassembly of the 1.10 build.

The game takes any saves from NX2 and converts them to NXA saves. Internally, this is done by
simply appending new data to the existing NX2 save file format. However, this does not keep the
format (backwards) compatible at all, as some missions have changed as well as how unlock flags
are processsed by the game, e.g. barrier unlock flags are entirely re-mapped.

#### Barriers

*EFxxx* identifiers are the official "mission step" identifiers used in the game's code and
assets. You can use these to search for the mission entries in the `mission.txt` file.

Total of 10 barriers to unlock. IDs denote game internal identifiers for each barrier.

* Yellow (Barrier ID 7)
  * Clear (nearly) all missions in Ladania
  * Mission step IDs: EF450 (Beat the Step / Ladania) to EF464 (Devil Castle / Ladania) without EF458 (NXSB / Ladania)
* Dark Green (Barrier ID 5)
  * Clear (nearly) all missions "on earth" which includes missions of the lands Caelum, Luman Ocean, Ladania, Pandonus and Arowahi
  * Mission step IDs: EF400 (Arriving / Caelum) to EF487 (More-Powerful 2 / Arowahi) without EF432 (* Yasangma 2-2 / Luman Ocean)
* ??? (Barrier ID 10)
  * Clear all missions in Caelum
  * Mission step IDs: EF400 (Arriving / Caelum) to EF424: Appear / Caelum
* ??? (Barrier ID 8)
  * Clear Turn Turn Turn! / Pandonus (EF445)
* ??? (Barrier ID 13)
  * Clear She Likes D / Pandonus (EF440)
  * Clear Lighthouse / Arowahi (EF483)
* ??? (Barrier ID 11)
  * Clear Wook! / Arowahi (EF466)
  * Clear Turn Turn Turn! / Pandonus (EF445)
  * Total world max clear percentage > ~61% (the math in the code is super weird resulting in a non-rounded floating point value)
* ??? (Barrier ID 6)
  * Clear More-Powerful 8 / Evgeneia Geist (EF599)
  * Clear WormHole / Dream's Kyma (EF598)
  * Clear WormHole / Stella's Maya (EF597)
  * Clear Galaxy / Infiniti Musica (EF596)
  * Clear * Ehne 2 / Infiniti Musica (EF595)
  * Clear ForWamm / Infiniti Musica (EF594)
* ??? (Barrier ID 4)
  * Clear Remember Me 4" / Parafora Psychi (EF631)
  * Clear Space City 2 / Parafora Psychi (EF632)
  * Clear WormHole / Stella's Maya (EF636)
  * Clear Dead End 2 / Cosmos Spatium (EF637)
* ??? (Barrier ID 12)
  * Either have it already unlocked + some unknown condition
  * Or get more than 19% on World Max
* ??? (Barrier ID 9)
  * Accumulated clear percentages (see profile overview on USB load) of World Max, Arcade Station,
    Brain Shower and Special Zone >= 40%
  * Which means that achieving >= 40% on one station alone, e.g. World Max, also works
