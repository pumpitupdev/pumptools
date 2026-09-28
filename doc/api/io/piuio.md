# PIUIO API
This contains various MK6 USB PIUIO related tools and libraries. The hardware was first introduced with Exceed 2 and
is supported by all games up to this day.

## Pumptools PIUIO API implementations
The following libraries implement pumptool's PIUIO API interface. Set the configuration value `patch.piuio.emu_lib`
in your hook configuration to point to the library you want to use, e.g. `patch.piuio.emu_lib=./ptapi-io-piuio-null.so`.

The path can select a library installed beside one game or a library in a shared folder used by multiple games. For
keyboard input, `patch_hook_main_loop.x11_input_handler` must point to the same library.

The keyboard and joystick implementations first load their mapping file from the folder containing the selected
library. If that fails, they fall back to the folder containing the game's `piu` executable. This supports both of the
following layouts:

* Per-game: place the library and mapping file beside `piu` and use a relative path such as
  `patch.piuio.emu_lib=./ptapi-io-piuio-keyboard.so`.
* Shared: place the library and mapping file in one common folder and configure every game with the same absolute path,
  such as `patch.piuio.emu_lib=/opt/pumptools/io/ptapi-io-piuio-keyboard.so`.

When mapping files exist in both locations, the file beside the selected library takes precedence.

### Null: ptapi-io-piuio-null.so
A null implementation for API dummy testing against libraries/applications calling API implementations.

### Real PIUIO: ptapi-io-piuio-real.so
Support for a real USB PIUIO. This library is not useful for games that already support the hardware natively.
However, games that require an emulation layer need this to run on real PIUIO hardware, e.g. Exceed (1).

### Joystick/Gamepad: ptapi-io-piuio-joystick.so
Support all (USB) Joysticks and Gamepads that are detected by the Linux kernel. This uses the Kernel's joystick API.

Button mappings are configured by using the `ptapi-io-piuio-joystick-conf` (this is an executable!) tool. You might have
to `chmod +x ptapi-io-piuio-joystick-conf` it prior being able to run it: `./ptapi-io-piuio-joystick-conf`. Run the tool
and follow the instructions. Once successfully completed, a `piuio-joystick-conf.bin` configuration file is located in
the same folder. Keep it beside `ptapi-io-piuio-joystick.so`, either in the game's folder or in a shared folder.

Configure your `hook.conf` file accordingly:
```
patch.piuio.emu_lib=./ptapi-io-piuio-joystick.so
```

### Keyboard: ptapi-io-piuio-keyboard.so
Support for inputs via your standard keyboard using the X11 API.

Button mappings are configured by using the `ptapi-io-piuio-keyboard-conf` (this is an executable!) tool. You might have
to `chmod +x ptapi-io-piuio-keyboard-conf` it prior being able to run it: `./ptapi-io-piuio-keyboard-conf`. Run the tool
and follow the instructions. Once successfully completed, a `piuio-keyboard-conf.bin` configuration file is located in
the same folder. Keep it beside `ptapi-io-piuio-keyboard.so`, either in the game's folder or in a shared folder.

Configure your `hook.conf` file accordingly:
```
patch.piuio.emu_lib=./ptapi-io-piuio-keyboard.so
patch_hook_main_loop.x11_input_handler=./ptapi-io-piuio-keyboard.so
```

Setting the key `patch_hook_main_loop.x11_input_handler` is important. Otherwise, the library does not receive any
input events from X11 and your configured keyboard input does not work in the game.

## Pumptools PIUIO API tester: ptapi-io-piuio-test
The tool *ptapi-io-piuio-test* lets you easily test and debug your library implementing pumptool's piuio API without
having to setup and/or run any games.

## Development
The [piuio header](../../src/api/ptapi/io/piuio.h) contains all the prototypes to be implemented by your library to
provide your own implementation for a PIUIO. Checkout the header file for documentation.

Checkout the [stub implementation](../../dist/api/ptapi-io-piuio-stub.c) to get started with a simple example.
Instructions are located in the header comment of the file.
