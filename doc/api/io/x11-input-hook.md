# X11 input hook API
The [x11-input-hook header](../../src/api/ptapi/io/x11-input-hook.h) allows you to hook into the main loop of the game
to receive X11 keyboard inputs. This might be useful for implementations of other APIs, e.g. PIUIO, to trigger PIUIO
inputs when pressing keys on the keyboard.

You have to configure your `hook.conf` accordingly that your implementation gets loaded by the hook library:
```
patch_hook_main_loop.x11_input_handler=./ptapi-io-my-x11-input-hook.so
```