# PIUBTN API
This contains various PIUBTN related tools and libraries. PIUBTN is the button board that was first introduced with
Pump PRO and officially works as an optional peripheral on all classic pump games starting Fiesta.

## Pumptools PIUBTN API implementations
The following libraries implement pumptool's PIUBTN API interface. Set the configuration value `patch.piubtn.emu_lib`
in your hook configuration to point to the library you want to use, e.g.
`patch.piubtn.emu_lib=./ptapi-io-piubtn-null.so`.

### Null: ptapi-io-piubtn-null.so
A null implementation for API dummy testing against libraries/applications calling API implementations.

### Real PIUIO: ptapi-io-piubtn-real.so
Support for a real USB PIUBTN. This library is not useful for games that already support the hardware natively.
However, games that require an emulation layer need this.

### Joystick/Gamepad: ptapi-io-piubtn-joystick.so
Support all (USB) Joysticks and Gamepads that are detected by the Linux kernel. This uses the Kernel's joystick API.

Button mappings are configured by using the `ptapi-io-piubtn-joystick-conf` (this is an executable!) tool. You might
have to `chmod +x ptapi-io-piubtn-joystick-conf` it prior being able to run it: `./ptapi-io-piubtn-joystick-conf`. Run
the tool and follow the instructions. Once successfully completed, a `piubtn-joystick-conf.bin` configuration file is
located in the same folder. This must be located next to the `ptapi-io-piubtn-joystick.so` file which should be located
next to your `piu` executable.

Configure your `hook.conf` file accordingly:
```
patch.piubtn.emu_lib=./ptapi-io-piubtn-joystick.so
```

### Keyboard: ptapi-io-piubtn-keyboard.so
Support for inputs via your standard keyboard using the X11 API.

Button mappings are configured by using the `ptapi-io-piubtn-keyboard-conf` (this is an executable!) tool. You might
have to `chmod +x ptapi-io-piubtn-keyboard-conf` it prior being able to run it: `./ptapi-io-piubtn-keyboard-conf`. Run
the tool and follow the instructions. Once successfully completed, a `piubtn-keyboard-conf.bin` configuration file is
located in the same folder. This must be located next to the `ptapi-io-piubtn-keyboard.so` file which should be located
next to your `piu` executable.

Configure your `hook.conf` file accordingly:
```
patch.piubtn.emu_lib=./ptapi-io-piubtn-keyboard.so
patch_hook_main_loop.x11_input_handler=./ptapi-io-piubtn-keyboard.so
```

Setting the key `patch_hook_main_loop.x11_input_handler` is important. Otherwise, the library does not receive any
input events from X11 and your configured keyboard input does not work in the game.

## Pumptools PIUBTN API tester: ptapi-io-piubtn-test
The tool *ptapi-io-piubtn-test* lets you easily test and debug your library implementing pumptool's piubtn API without
having to setup and/or run any games.

## Development
The [piubtn header](../../src/api/ptapi/io/piubtn.h) contains all the prototypes to be implemented by your library to
provide your own implementation for a PIUBTN. Checkout the header file for documentation.

Checkout the [stub implementation](../../dist/api/ptapi-io-piubtn-stub.c) to get started with a simple example.
Instructions are located in the header comment of the file.