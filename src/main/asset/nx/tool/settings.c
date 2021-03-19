/**
 * Tool for NX settings files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asset/nx/lib/settings.h"
#include "asset/nx/lib/util.h"

static int main_settings(char *cmd, char *file);

int main(int argc, char **argv)
{
  int ret;
  char *cmd;
  char *type;
  char *file;

  if (argc < 4) {
    printf(
        "Usage: %s [cmd: new, dump, unlock] [type: ini, mrank, rank] [file]\n",
        argv[0]);
    return -1;
  }

  cmd = argv[1];
  type = argv[2];
  file = argv[3];

  if (!strcmp(type, "ini")) {
    ret = main_settings(cmd, file);
  } else {
    fprintf(stderr, "Unknown file type %s\n", type);
    ret = -1;
  }

  return ret;
}

static int main_settings(char *cmd, char *file)
{
  int ret;

  ret = 0;

  if (!strcmp(cmd, "new")) {
    struct asset_nx_settings *settings = asset_nx_settings_new();

    asset_nx_settings_finalize(settings);

    if (!asset_nx_util_save_settings_to_file(file, settings)) {
      fprintf(stderr, "Creating nx settings file %s failed\n", file);
      ret = -1;
    }

    free(settings);
  } else {
    struct asset_nx_settings *settings =
        asset_nx_util_load_settings_from_file(file);

    if (!settings) {
      fprintf(stderr, "Loading %s failed", file);
      ret = -1;
    }

    if (!strcmp(cmd, "dump")) {
      char *str = asset_nx_settings_to_string(settings);

      printf("----------------- settings -----------------\n%s\n", str);

      free(str);
    } else if (!strcmp(cmd, "unlock")) {
      asset_nx_settings_unlock_all(settings);

      asset_nx_settings_finalize(settings);

      if (!asset_nx_util_save_settings_to_file(file, settings)) {
        fprintf(stderr, "Saving nx settings file %s failed\n", file);
        ret = -1;
      }
    } else {
      fprintf(stderr, "Unknown command %s\n", cmd);
      ret = -1;
    }

    free(settings);
  }

  return ret;
}