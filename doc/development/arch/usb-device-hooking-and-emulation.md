# Usb device hooking and emulation

This document gives you an overview of how pumptools handles hooking of usb devices. To be more specific, it does that
by hooking the API calls of the libusb 0.1 library. Why 0.1 and not 1.0? Because that's the version all pump games have
used so far. However, pumptools is using the libusb 1.0 API for any communication with real usb devices because 0.1 is
considered deprecated for quite a while now.

## Architecture: PIUIO example

The following ASCII diagram depicts the various modules and how they are involved in the flow for handling the USB PIUIO.

```text
                                                                        +--------------+  +--------------+
                                                                        | usb-init-fix |  | usb-hook-mon |
                                                                        | module       |  | module       |
              One update cycle:                                         +--------------+  +--------------+
                4x ctrl out                                                     ^                ^
                4x ctrl in                                                      |                |                                                                                                                +----------+
                interleaved      +-----------+             +---------+          |                |                                   +------------+                                                               | ptapi    |
+----------+    per sensor       |           |  forward    |         |          |                |       forward                     |            |                                                          +--->| piuio    |
| Game PIU +-------------------->|           +------------>|         +----------+----------------+---------------------------------->|            |                                                          |    | keyboard |
+----------+                     |           |             |         |          |                |                                   |            |                                                          |    +----------+
                                 |           |             |         +----------+                |                                   |            |     +----------+      +--------+      +---------------+  |
              One update cycle:  | libusb0.1 |             | usbhook |                           |                     4x ctrl out   | piuio-exit |     | usb-emu  |      | piuio  |      | api pumptools +--+
               1x ctrl out       |    API    |             | module  +---------------------------+                     4x ctrl in    | module     +---->| module   +----->| module +----->| piuio         +--+
               (in data on call  |           |             |         |                              +-------------+    interleaved   |            |     +----------+      +--------+      +---------------+  |
+----------+   return in buffer) |           |  forward    |         |           forward            | piuio-khack |    per sensor    |            |                                                          |     +-------+
| Game PRO +-------------------->|           +------------>|         +----------------------------> | module      +----------------->|            |                                                          |     | ptapi |    +-----------+
+----------+                     |           |             |         |                              +-------------+                  |            |                                                          +---->| piuio +--->| libusb1.0 |
                                 +-----------+             +---------+                                                               +------------+                                                                | real  |    +-----------+
                                                                                                                                                                                                                   +-------+
```

Rough idea of how this works:

* Classic PIU games issue 8 calls via the libusb 0.1 API
* PRO instead issues only a single call to reduce overhead
    * On a stock PRO OS and setup, this is intercepted by a kernel module
* All calls that go to libusb0.1 are intercepted by pumptools's `usbhook` module
    * A separate `usb-init-fix` module takes care of some quirks when calling init more than once
    * `usb-hook-mon` is used for logging all intercepted calls for tracing and debugging purpose
* In general, any calls are forwaded by an internal API exposed by `usbhook` to any hooks registering for them
* `piuio-khack` intercepts the single ctrl out call issued on PRO by the PIUIO device. It converts it to the call pattern
equivalent to the classic PIU games. This allows us to transparently handle them further downstream
* Next comes the `piuio-exit` module which checks whether the the service and test buttons are pressed on a ctrl-transfer
level (allowing this to be used without any emulation layer by real devices) to exit the process
* The `usb-emu` module creates an abstraction layer for the libusb 0.1 API calls. Furthermore, it allows for injecting
fake stub devices if the application requires IO hardware to be available to work
* The `piuio` module dispatches the in and out ctrl calls, extracts the piuio data from the buffers and hooks it up
to pumptools's piuio API layer
* For the API layer, we can have different implementations, e.g. keyboard or a PIUIO driver talking to a real IO using
libusb 1.0