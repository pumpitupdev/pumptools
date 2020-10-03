/**
 * Fix a quirk with getgrnam_r in the asound library always failing when loading the asound.conf file and reading the
 * /etc/group file. For some reason, it cannot find the audio group entry even if it is available and can be read
 * by other applications just fine.
 *
 * Your typical asound.conf, which is loaded by libasound in a application that uses it, contains the following line:
 * defaults.pcm.ipc_gid audio
 *
 * Finding documentation about what this stuff does it difficult but judging by the name, this seems to be related to
 * setting up shared memory IPC for reading and writing data to the sound driver.
 * For some reason, this "fails" when the function getgrnam_r is called by libasound. The function itself returns
 * successful but with no result. This function reads the entry from /etc/group. libsound queries the "audio" group
 * in that case and even if the entry exists, the result is empty for some unknown reason.
 *
 * This module fixes that issue.
 */
#ifndef PATCH_ASOUND_FIX_H
#define PATCH_ASOUND_FIX_H

/**
 * Initialize the patch module
 */
void patch_asound_fix_init();

#endif
