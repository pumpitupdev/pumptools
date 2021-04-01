# Release history
## 1.11
* Hotfix: Remove experimental option for gfx scaling temporarily

## 1.10
* Refactor pumptools's usb device hooking backend
* Fix issues with piuio kernel hack in PRO/PRO 2
* Fix high CPU load issue due to 4x polling load on piuio API 
* Add architecture documentation about usb device handling
* Remove non-working window option from Exceed to Prime
* Add documentation clarifying window/full-screen handling on Linux
* NX2 game doc: Add all unlock codes for World Max
* Add experimental option for gfx scaling, see hook documentation
* Add hardware documentation: cabinet, PCB and manuals

## 1.09
* Include pro and pro2 hooks into actual public release
* Various documentation updates
* Minor bugfixes

## 1.08
* Documentation updates
* Public release, working hooks: mk3, exc, x2, zero, nx, nx2, pro, pro2

## 1.07
* Bugfix nx2hook usb thumb drives not being detected on all kernel versions correctly
* Include piubtn library and api (piu pro button board) in distribution for SGL
* Various documentation improvements
* Various minor bugfixes and improvements

## 1.06
* nx2hook: Add hook library for NX2
* pumpnet: Support usbprofiles over network for NX2
* Various bugfixes

## 1.05
* Minor improvements
* Improve log/debug output
* piueb: Add `valgrind` run mode for debugging
* Improve documentation
* ptapi/piuio: Bugfix resolving path to configuration file. Config file local to piuio library.
* usbhooking: Bugfix to make PIUIO work without using ptapi-io-piuio-real.so on newer games, Exceed 2, Zero, NX

## 1.04
* nxhook: Add hook library for NX
* Minor bugfixes

## 1.03
* zerohook: Add hook library for Zero
* Bugfix linking error ptapi piuio real impl
* Bugfix piueb environment variable order with LD_LOAD_LIBRARY
* Readme/documentation cleanup, merged duplicate documentation for hooks
* Various other bugfixes

## 1.02
* x2hook: Add hook library for Exceed 2
* Various bugfixes
* Documentation and code cleanup

## 1.01
* exchook: Add hook library for Exceed
* mk3hook: Add support for piuio emulation -> configurable keyboard and joystick supported
* Add game documentation: Trivia, unlocks and pad codes about all games released so far (check game-doc.zip)
* Update API documentation
* Add stub example for implementing your own piuio module
* Add configurable joystick/gamepad support for piuio emulation (see readme files for instructions)
* Add configurable keyboard support for piuio emulation (see readme files for instructions)
* Refactor pumptools API (ptapi) piuio and piubtn modules
* Build releases using docker container with ubuntu 16.04 to enhance compatibility with older libc versions
* piueb: Add commands "debug" and "strace" to start game with a gdb server or strace attached for quick debugging
* Various bugfixes

## 1.00
* Release of mk3hook for Linux ports
* Initial public release of binary distribution of pumptools