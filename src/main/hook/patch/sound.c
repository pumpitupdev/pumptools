#define LOG_MODULE "patch-sound"

#include <alsa/asoundlib.h>
#include <grp.h>
#include <sys/types.h>

#include "capnhook/hook/lib.h"

#include "util/log.h"

typedef int (*snd_pcm_open_t)(
    snd_pcm_t **pcmp, const char *name, snd_pcm_stream_t stream, int mode);

static snd_pcm_open_t patch_sound_real_snd_pcm_open;

static const char *patch_sound_dev_name;

int snd_pcm_open(
    snd_pcm_t **pcmp, const char *name, snd_pcm_stream_t stream, int mode)
{
  int ret = 0;
  int retry_count = 5;

  log_debug("snd_pcm_open: %s, stream %d, mode %d", name, stream, mode);

  if (!patch_sound_real_snd_pcm_open) {
    patch_sound_real_snd_pcm_open =
        (snd_pcm_open_t) cnh_lib_get_func_addr("snd_pcm_open");
  }

  if (patch_sound_dev_name) {
    log_info(
        "Opening sound device %s instead of %s", patch_sound_dev_name, name);

    name = patch_sound_dev_name;
  }

  /*
     Notes about exceed and how it fucked up using alsa:
     - Opening the same (hw:0) device multiple times without closing
     - Opening and closing the device on each effect play
     - No proper error handling on snd_pcm_open return
  */

  /* Using dmix instead of hw:0 on exceed, snd_pcm_open sometimes returns
     with a bad fd but after another retry, everything's fine */
  while (retry_count > 0) {
    ret = patch_sound_real_snd_pcm_open(pcmp, name, stream, mode);

    if (ret < 0) {
      retry_count--;
    } else {
      break;
    }
  }

  if (ret < 0) {
    log_error("snd_pcm_open failed: %s", snd_strerror(ret));
  }

  return ret;
}

void patch_sound_init(const char *dev_name)
{
  patch_sound_dev_name = dev_name;

  log_info("Initialized");
}