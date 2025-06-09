# Pumptools: The ultimate toolset to run all Linux based PIU games on any* kernel and distribution

Version: 1.13

[Release history](CHANGELOG.md)

This tool-set provides hook libraries that can be pre-loaded to run vanilla dumps of all Pump It Up games running on
Linux. This also includes the only exclusive MK5 hardware based game Exceed and the MK3 Linux ports.

## Documentation

Additional documentation on different topics related to pumptools are located in the `doc` subfolder
[here](doc/README.md).

For release distribution packages, you can find a `doc.zip` file inside the `pumptools.zip` package that contains all
documentation also referenced from here:

### Developers

* [API](doc/api/api.md): Pumptools public API documentation to write your own integrations with
  pumptools, e.g. IO
* [Development](doc/development/development.md): How to develop for and with pumptools, e.g.
  development environment setup, building
* [Architecture](doc/development/architecture.md): Describes the architecture of pumptools outlining the most important
  high level aspects

### Users

* [Hook library documentation](doc/hook/hook.md): User documentation how to setup and use the hook libraries
* [Hardware docs](https://github.com/Shizmob/arcade-docs): Boards, cabinets, IOs etc.
* [Game documentation](https://github.com/pumpitupdev/piu-game-docs): General game related stuff like pad codes, unlocks
  etc

## Hook libraries

### Features

* Run all supported games on recent hardware, Linux distributions and kernel versions
* Environment sandboxing to increase compatibility and portability
* API for hardware abstraction: Write your own drivers for IO devices
* Full dongle emulation in software
* Works with **vanilla** data, no hex edits
* Various (bug)fixes and quality of live improvements for many games

### Games and latest versions supported

All hooks share a major set of features provided by pumptools. Therefore, common topics are covered in the
[main hook readme](doc/hook/hook.md). Start there when you are setting up a game with pumptools. Additionally, refer to
each dedicated hook readme for further important and required information to successfully setup and run the games.

The following games are supported and tested with the latest versions available. Earlier version might also be support
and work because of the nature the hooks are implemented.

* [MK3 Linux Ports: 1st to Prem 3/Prex 3](doc/hook/mk3hook.md): MK3 Linux ports (there is only one version officially
available per game version)
* [Exceed](doc/hook/exchook.md): 20040408
* [Exceed 2](doc/hook/x2hook.md): 102
* [Zero](doc/hook/zerohook.md): 103
* [NX](doc/hook/nxhook.md): 108
* [Pro](doc/hook/prohook.md): R5 nodongle
* [NX2](doc/hook/nx2hook.md): 154
* [Pro2](doc/hook/pro2hook.md): R5 nodongle
* [NXA](doc/hook/nxahook.md): 110
* [Fiesta](doc/hook/fsthook.md): 120
* [FiestaEX](doc/hook/fexhook.md): 151
* [Fiesta 2](doc/hook/f2hook.md): 161
* [Prime](doc/hook/prihook.md): 122

### Prepare your operating system

Before you start installing and configuring anything of pumptools, read [this readme](doc/hook/os.md) on how to prepare
your operating system and environment. Please also note that not every Linux distribution and different versions were
tested and verified working.

### Get and prepare game data

You need **vanilla**, i.e. non-tempered and non-decrypted, game assets as well as non-tempered executable binaries
which can be pulled from legit and clean physical disks or images of them.

For setting up the games, have a look at the dedicated readme files for
[each hook and game](#games-and-latest-versions-supported).

### Setup hook libraries

**READ THE WHOLE README OF THE HOOK BUILD YOU WANT TO USE FIRST BEFORE DOING ANYTHING!!!**

The readme files go into great detail how to setup things with various potential relevant aspects spread across the
document. Being aware of them will save you a lot of puzzling and time. Furthermore, read the FAQ/troubleshooting
section before as well to be aware of any known issues when you encounter them.

## Building

Make sure you have the [required tools](doc/development/development.md) installed. Once that's done, simply run

```shell script
make build
```

to build it on your current host from the root directory of this project.

The build output is located in the `build` subdirectory. The built binaries can be found in `build/bin` and a
distribution package called `pumptools.zip` can be found in the `build` subfolder.

You can also build in a docker container. The container provides the necessary build environment and also ensures 
[compatibility regarding glibc versions](doc/development/development.md#compatibility-issues-with-newer-glibc-versions).

```shell script
make build-docker
```

The build output from the docker image is located in `build/docker`.

## License

Source code license is the Unlicense; you are permitted to do with this as thou wilt. For details, please refer to the
[LICENSE file](LICENSE) included with the source code.
