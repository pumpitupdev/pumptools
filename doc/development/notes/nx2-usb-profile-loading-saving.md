# Notes about how NX2 handles saving and loading usb profile data
Executable of v1.54.

This document outlines the relevant parts regarding how NX2 loads and stores profiles and how this had an impact on the
design of the patch module `net-profile`, that hooks up usb profile loading over socket to a remote server.

## Syscall flow
The game calls the following system calls in order per player, here player 1 (id 0), when loading usb profile data
(assume all calls are successful here):
```c
// Simplified pseudo-like C code omitting non relevant parameters
fopen("/mnt/0/nx2save.bin")
fread(fd_save) // stats area
fread(fd_save) // encrypted area
fclose(fd_save)

fopen("/mnt/0/nx2rank.bin")
fread(fd_rank) // encrypted area
fclose(fd_rank)
```

For saving the profile, this is the call flow:
```c
// Simplified pseudo-like C code omitting non relevant parameters
fopen("/mnt/0/nx2save.bin")
fseek(fd_save) // beginning
fwrite(fd_save) // stats area
fwrite(fd_save) // encrypted area
fclose(fd_save)

fopen("/mnt/0/nx2rank.bin")
fseek(fd_rank) // beginning
fwrite(fd_rank) // encrypted area
fclose(fd_rank)
```

We can see that in both scenarios, for loading and saving, the game first takes care of a single file before moving on
to the next one.

There is another flow that is slightly different when playing World Max. After playing a song in World Max, the game
writes the changes of the `nx2save.bin` file back:
```c
// Simplified pseudo-like C code omitting non relevant parameters
fopen("/mnt/0/nx2save.bin")
fseek(fd_save) // beginning
fwrite(fd_save) // stats area
fwrite(fd_save) // encrypted area
fclose(fd_save)
```

## Pumpnet RPCs
The above set the requirements for how pumpnet needs to handle file loading and storing.

### First solution
The initial solution was combining the loading and storing process of both files, `nx2save.bin` and `nx2rank.bin`
into a single download/upload call.

Load phase:
```c
// Simplified pseudo-like C code omitting non relevant parameters
fopen("/mnt/0/nx2save.bin")
pumpnet_download_profiles()
fread(fd_save) // read from buffered data
fread(fd_save) // read from buffered data
fclose(fd_save)

fopen("/mnt/0/nx2rank.bin") // skip, data already buffered
fread(fd_rank) // read from buffered data
fclose(fd_rank) // clear all buffers and reset internal state
```

Save phase:
```c
// Simplified pseudo-like C code omitting non relevant parameters
fopen("/mnt/0/nx2save.bin")
pumpnet_download_profiles()
fseek(fd_save) // seek for buffers
fwrite(fd_save) // write to buffers
fwrite(fd_save) // write to buffers
fclose(fd_save) // skip, do nothing

fopen("/mnt/0/nx2rank.bin") // skip, data already buffered
fseek(fd_rank) // seek for buffers
fwrite(fd_rank) // write to buffers
pumpnet_upload_profiles()
fclose(fd_rank) // clear all buffers and reset internal state
```

However, with the additional flow during World Max, this solution didn't work and broke internal state handling.
Modifying this solution to incorporate saving just the `nx2save.bin` file wasn't feasible with the above architecture.

### Second solution
Therefore, the pumpnet backend was re-designed to operate on on a single file level and separate for saving and
loading.

Load phase:
```c
// Simplified pseudo-like C code omitting non relevant parameters
fopen("/mnt/0/nx2save.bin")
pumpnet_download_save()
fread(fd_save) // read from buffered data
fread(fd_save) // read from buffered data
fclose(fd_save)
pumpnet_clear_save_cache()

fopen("/mnt/0/nx2rank.bin")
pumpnet_download_rank()
fread(fd_rank) // read from buffered data
fclose(fd_rank)
pumpnet_clear_rank_cache()
```

Save phase:
```c
// Simplified pseudo-like C code omitting non relevant parameters
fopen("/mnt/0/nx2save.bin")
pumpnet_download_save()
fseek(fd_save) // seek for buffers
fwrite(fd_save) // write to buffers
fwrite(fd_save) // write to buffers
fclose(fd_save)
pumpnet_upload_save()
pumpnet_clear_save_cache()

fopen("/mnt/0/nx2rank.bin")
fseek(fd_rank) // seek for buffers
fwrite(fd_rank) // write to buffers
fclose(fd_rank)
pumpnet_upload_rank()
pumpnet_clear_rank_cache()
```

Buffer modifications are detected when `fwrite` is called and flagged accordingly that the module is aware of having
to write back dirty data on the call to `fclose`.

State handling is also simplified as it only needs to be taken care of on a file level between `fopen` and `fclose`
calls and not on a multi-file level.

This solution is definitely more robust and flexible than the previous one that operated on a full profile level.
However, the call overhead to the remote server is four times higher compared to the previous solution. Considering
that these calls are not being issued on a high frequency, server load is unlikely to be an issue.