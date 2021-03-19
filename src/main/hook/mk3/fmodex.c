#define LOG_MODULE "mk3hook-fmodex"

#include <fmodex/fmod.h>
#include <fmodex/fmod_errors.h>

#include "capnhook/hook/lib.h"

#include "hook/mk3/fmodex.h"

#include "util/log.h"
#include "util/str.h"

typedef FMOD_RESULT (*FMOD_System_Init_t)(
    FMOD_SYSTEM *system,
    int maxchannels,
    FMOD_INITFLAGS flags,
    void *extradriverdata);

static FMOD_System_Init_t mk3hook_fmodex_real_FMOD_System_Init;

static bool mk3hook_fmodex_debug_output;
static const char *mk3hook_fmodex_sound_device;

FMOD_RESULT FMOD_System_Init(
    FMOD_SYSTEM *system,
    int maxchannels,
    FMOD_INITFLAGS flags,
    void *extradriverdata)
{
  if (!mk3hook_fmodex_real_FMOD_System_Init) {
    mk3hook_fmodex_real_FMOD_System_Init =
        (FMOD_System_Init_t) cnh_lib_get_func_addr("FMOD_System_Init");
  }

  FMOD_RESULT res;

  unsigned int version;

  res = FMOD_System_GetVersion(system, &version);

  if (res != FMOD_OK) {
    log_error(
        "FMOD_System_GetVersion failed %d: %s",
        res,
        FMOD_ErrorString((FMOD_RESULT) res));
    return res;
  }

  log_info(
      "fmodex version: %d.%d.%d",
      (version >> 16) & 0xFFFF,
      (version >> 8) & 0xFF,
      version & 0xFF);

  if (mk3hook_fmodex_debug_output) {
    res = FMOD_Debug_SetLevel(FMOD_DEBUG_ALL);

    if (res != FMOD_OK) {
      log_error(
          "FMOD_Debug_SetLevel failed %d: %s",
          res,
          FMOD_ErrorString((FMOD_RESULT) res));
      return res;
    }
  }

  res = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_ALSA);

  if (res != FMOD_OK) {
    log_error(
        "FMOD_System_SetOutput failed %d: %s",
        res,
        FMOD_ErrorString((FMOD_RESULT) res));
    return res;
  }

  FMOD_OUTPUTTYPE output_type;

  res = FMOD_System_GetOutput(system, &output_type);

  if (res != FMOD_OK) {
    log_error(
        "FMOD_System_GetOutput failed %d: %s",
        res,
        FMOD_ErrorString((FMOD_RESULT) res));
    return res;
  }

  log_info("Output type: %d", output_type);

  int num_drivers;
  res = FMOD_System_GetNumDrivers(system, &num_drivers);

  if (res != FMOD_OK) {
    log_error(
        "FMOD_System_GetNumDrivers failed %d: %s",
        res,
        FMOD_ErrorString((FMOD_RESULT) res));
    return res;
  }

  log_info("Num available drivers %d", num_drivers);

  char buffer[4096];
  FMOD_GUID guid;
  int selected_driver = -1;

  for (int i = 0; i < num_drivers; i++) {
    res = FMOD_System_GetDriverInfo(system, i, buffer, sizeof(buffer), &guid);

    if (res != FMOD_OK) {
      log_error(
          "FMOD_System_GetDriverInfo failed %d: %s",
          res,
          FMOD_ErrorString((FMOD_RESULT) res));
      break;
    }

    log_info("Driver %d: %s", i, buffer);

    if (mk3hook_fmodex_sound_device &&
        !strcmp(buffer, mk3hook_fmodex_sound_device)) {
      selected_driver = i;
    }
  }

  if (selected_driver == -1) {
    log_info(
        "Selecting default driver (0) because either the selected driver was "
        "not found or no driver was "
        "selected");
    selected_driver = 0;
  } else {
    log_info(
        "Found selected driver %s, id %d",
        mk3hook_fmodex_sound_device,
        selected_driver);
  }

  res = FMOD_System_SetDriver(system, selected_driver);

  if (res != FMOD_OK) {
    log_error(
        "FMOD_System_SetDriver failed %d: %s",
        res,
        FMOD_ErrorString((FMOD_RESULT) res));
    return res;
  }

  res = FMOD_System_SetSpeakerMode(system, FMOD_SPEAKERMODE_STEREO);

  if (res != FMOD_OK) {
    log_error(
        "FMOD_System_SetSpeakerMode failed %d: %s",
        res,
        FMOD_ErrorString((FMOD_RESULT) res));
    return res;
  }

  return mk3hook_fmodex_real_FMOD_System_Init(
      system, maxchannels, flags, extradriverdata);
}

void mk3hook_fmodex_init(bool debug_output, const char *sound_device)
{
  mk3hook_fmodex_debug_output = debug_output;
  mk3hook_fmodex_sound_device = sound_device;

  log_info("Initialized");
}