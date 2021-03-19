# nx2hook: NX2
This readme covers any matters that are relevant for this hook, only. Anything that applies to **all** hooks is covered
in a [main hook readme file](../hook.md) including general data setup and a quick start guide.

## Additional notable features
* Removed USB flash drive vendor lock, i.e. use _ANY_ USB flash drive to store game profiles
* Auto generate new profiles if no profile is found on the connected USB flash drive

## Versions supported
All known versions supported.

## Dependencies
Make sure to read the different methods of dependency resolution available in the [main hook readme file](../hook.md),
first.

Note: Game is 32-bit, so you need to install the 32-bit versions of the dependencies!

The following **direct** dependencies (cmd: `readelf -d piu`) are required:
* libfreetype.so.6
* librt.so.1
* libGL.so.1
* libGLU.so.1
* libusb-0.1.so.4
* libpthread.so.0
* libXxf86vm.so.1
* libpng12.so.0
* libasound.so.2
* libmad.so.0
* libgcc_s.so.1
* libc.so.6
* libm.so.6
* libX11.so.6
* libstdc++.so.6
* libz.so.1

As for method 1, when using Ubuntu, the dependencies can be found in the following packages:
* libc-bin (or gcc-multilib on a 64-bit platform)
* libx11-6
* zlib1g
* libusb-0.1-4
* libasound2

Additionally, `nx2hook` needs the following **direct** dependencies for pumpnet:
* libcurl-gnutls.so.4

Ubuntu packages for `nx2hook` dependencies for pumpnet:
* libcurl4-gnutls

Further indirect dependencies are needed but should be taken care of automatically when using a package manager to
install the direct dependencies.

## Data setup
In additional to the [general information applying to **all** hooks](../hook.md#data-setup), this game requires all
files and folders from the original `game` folder to be in **UPPERCASE** on a case-sensitive file system with the
exception of `*.txt` and `*.ttf` files in the root of game. Further game asset files and folders from cramfs need to be
copied to the `game` directory. `nx.ttf`, `nxcn.ttf`, `nxpt.ttf`, `nxtw.ttf` and `mission.txt` must be **lowercase** but
`SCRIPT` and its contents must be **UPPERCASE**.

## Pumpnet setup
Instead of using plain usb profiles like on the vanilla version without pumptools (which is still possible with
pumptools), pumptools provides a patch module to upload/download usb profiles to/from a remote server over TCP IP
networks.

This feature can be enabled by setting the following configuration properties in the `hook.conf` file:
```text
patch.net_profile.server=<put the server address of the pumpnet server here>
patch.net_profile.machine_id=<put your machine id that's registered with the server here>
```

If the network offers or even requires secure communication using https, you should have received a package with a
client certificate, a client key and a certificate authority bundle. Place the files named `ca-bundle-crt.pem`,
`client-crt.pem` and `client-key.pem` in a folder, e.g. `cert` next to the `piu` executable and configure the hook
to use these for the encrypted communication by setting the property key `patch.net_profile.cert_dir_path`
accordingly, e.g. `patch.net_profile.cert_dir_path=./cert`. Note: The server URL that you set for the properties key
`patch.net_profile.server` has to be a https based URL in order to work correctly, e.g. `https://localhost`. Your
network provided of your choice should provide you the URL you have to use.

When everything's configured correctly, the log tells you so:
```text
Initialized pumpnet for game 20, serveraddr localhost:8080, main endpoint version /usbprofile/v1
Initialized: game 20, server XXXXXXX, machine id XXXXXXX
```

How you acquire an address to a remote server and a machine ID is out of this document's scope.

Once these parameters are present, pumptools is looking for a file called `pumpnet.bin` on connected usb sticks. As
long as this file is in the root directory of your usb drive, the game will ignore the the regular `nx2save.bin` and
`nx2rank.bin` files and always try to connect to the remote server. When you remove `pumpnet.bin`, it will pick up the
local profiles and not connect to the server, even if enabled.

`pumpnet.bin` contains an identifier for the player to login. This file needs to be provided by the network service
somehow. The how is out of the scope of this document.

Ensure you have mapped the usb ports in the game correctly. Go to the operator menu and the "usb drive" item. Plug 
**exactly one** usb thumb drive into the usb port you want to assign as P1 and hit the test button. The menu item
should change from `---` to a numerical value, e.g. `03:00` which describes the bus and port the drive got detected on.
Repeat this for P2.

When you start the game, have your usb stick plugged in and it is recognized correctly by the game, the log output
tells you if it found the `pumpnet.bin` file and connected to the server correctly for downloading your profile data:
```text
Found pumpnet profile file /mnt/0/pumpnet.bin
Profile file player 0, file_type 0, refId XXXXXXXXXXXXXX downloading from server...
Downloading file player 0, file_type 0, refId XXXXXXXXXXXXXX successful
```

If it cannot connect to the server, the log tells you that as well and retries to connect to it a few times before
giving up:
```text
Performing curl request failed: Couldn't connect to server
Failed, http code 0, retrying (0)...
...
```

## Troubleshooting and FAQ
Make sure to also check the
[troubleshooting and FAQ section of the main hook readme](../hook.md#troubleshooting-and-faq). This covers various
things that apply to **all** hooks. The following sub-sections apply mainly to this hook.

### How do I unlock 'Tell Me' on the non korean version?
Put an empty file called `KEY.LUA` into the `game/SCRIPT` folder and change the language to korean in the operator menu.
This loads the korean version of the game (yes, that's how it actually worked originally) and 'Tell Me' will be
available.

Note: You can't switch back to English unless you delete the file `game/SCRIPT/KEY.LUA`.

### When using pumpnet, the game does not detect any usb sticks
Make sure that you have configured the usb ports in the game's operator menu `USB DRIVE` menu item. For instructions,
refer to the [above section](#pumpnet-setup).

### When using pumpnet, the game tells me to register my usb drive on login
If you see the message "Please REGISTER your USB drive at www.piugame.com prior to using the product." when logging in
with pumpnet enable, your login got rejected for one of the following reasons:
* Your machine ID is invalid or not whitelisted on the server
* Your player ID is invalid or no such player exists on the server
* No NX2 profile was created for the player ID on the server

On any of the above cases, verify that you ensured you have configured everything correctly on the user web interface
of pumpnet. Otherwise, contact the server administrator.

### When using pumpnet, the game apparently gets stuck on the usb drive login screen
This can happen if the game cannot reach the server or on some other errors that might be temporarily. Therefore, the
game retries for up to ten times currently to reach the server or complete an outstanding operation. This is also
reflected in the logs with warning messages telling you the game is retrying.

### Curl requests fail: Problem with the SSL CA cert
When you get the following error message:
```text
Performing curl request failed: Problem with the SSL CA cert (path? access rights?)
```

Make sure the path you configured for the property key `patch.net_profile.cert_dir_path` is pointing to an existing
directory containing the files `ca-bundle-crt.pem`, `client-crt.pem` and `client-key.pem` that you received from the
the network you are trying to connect to.

### Curl requests fail: SSL peer certificate or SSH remote key was not OK
When you get the following error message:
```text
Performing curl request failed: SSL peer certificate or SSH remote key was not OK
```

Make sure that you have the files `ca-bundle-crt.pem`, `client-crt.pem` and `client-key.pem` that you have received
from the network you are trying to connect to placed in a directory. The path to the **directory** needs to be set in 
the hook configuration file by setting the key `patch.net_profile.cert_dir_path`, e.g. 
`patch.net_profile.cert_dir_path=./cert` if the files are placed next to the `piu` exec in the folder `cert`.