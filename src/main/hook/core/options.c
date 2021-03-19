#include "hook/core/options.h"

#include "util/options.h"

#define HOOK_CORE_OPTIONS_STR_PATCH_UTIL_LOG_FILE "util.log.file"
#define HOOK_CORE_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL "util.log.level"

const struct util_options_def hook_core_options_def[] = {
    {
        .name = HOOK_CORE_OPTIONS_STR_PATCH_UTIL_LOG_FILE,
        .description = "Print the log output to the specified file",
        .param = 'o',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "pumptools.log",
    },
    {
        .name = HOOK_CORE_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL,
        .description = "Set the log level (0-4)",
        .param = 'l',
        .type = UTIL_OPTIONS_TYPE_INT,
        .default_value.i = LOG_LEVEL_DEBUG,
    },
};

const struct util_options_defs hook_core_options_defs = {
    .usage_header = "",
    // Just use something because this is not relevant/required here
    .usage_param = '\r',
    .defs = hook_core_options_def,
    .ndefs = lengthof(hook_core_options_def)};

void hook_core_options_init(
    int argc, char **argv, struct hook_core_options *options)
{
  log_assert(argv);
  log_assert(options);

  struct util_options_opts *options_opt;

  util_options_init(argc, argv);
  options_opt = util_options_get(&hook_core_options_defs);

  if (!options_opt) {
    return;
  }

  options->log.file = util_options_get_str(
      options_opt, HOOK_CORE_OPTIONS_STR_PATCH_UTIL_LOG_FILE);
  options->log.level = (enum util_log_level) util_options_get_int(
      options_opt, HOOK_CORE_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL);
}