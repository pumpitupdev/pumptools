#define LOG_MODULE "util-options"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/defs.h"
#include "util/fs.h"
#include "util/hex.h"
#include "util/log.h"
#include "util/mem.h"
#include "util/options.h"
#include "util/str.h"

static struct util_options_opts* util_options_get2(int argc, char** argv,
    const struct util_options_defs* option_defs);
static bool util_options_init_from_args(struct util_options_opts* options,
    const struct util_options_defs* option_defs, int argc, char** argv);
static bool util_options_init_from_file(struct util_options_opts* options,
    const struct util_options_defs* option_defs, char* file_path);
static bool util_options_write_default_options_file(
        const struct util_options_defs* option_defs, char* file_path);

static int util_options_argc;
static char** util_options_argv;

void util_options_init(int argc, char** argv)
{
    util_options_argc = argc;
    util_options_argv = argv;
}

struct util_options_opts* util_options_get(
        const struct util_options_defs* option_defs)
{
    struct util_options_opts* options;

    options = util_options_get2(util_options_argc, util_options_argv,
        option_defs);

    if (!options) {
        exit(0);
    }

    return options;
}

int util_options_get_int(const struct util_options_opts* opts,
        const char* name)
{
    for (uint32_t i = 0; i < opts->entries; i++) {

        if (!strcmp(opts->defs->defs[i].name, name)) {

            if (opts->defs->defs[i].type != UTIL_OPTIONS_TYPE_INT) {
                log_warn("Option %s is not of type int", name);
                return 0;
            }

            if (opts->values[i].avail) {
                return opts->values[i].value.i;
            } else {
                return opts->defs->defs[i].default_value.i;
            }
        }
    }

    log_warn("Could not find int option %s", name);
    return 0;
}

double util_options_get_double(const struct util_options_opts* opts, const char* name)
{
    for (uint32_t i = 0; i < opts->entries; i++) {

        if (!strcmp(opts->defs->defs[i].name, name)) {

            if (opts->defs->defs[i].type != UTIL_OPTIONS_TYPE_DOUBLE) {
                log_warn("Option %s is not of type double", name);
                return 0;
            }

            if (opts->values[i].avail) {
                return opts->values[i].value.d;
            } else {
                return opts->defs->defs[i].default_value.d;
            }
        }
    }

    log_warn("Could not find double option %s", name);
    return 0;
}

const char* util_options_get_str(const struct util_options_opts* opts,
        const char* name)
{
    for (uint32_t i = 0; i < opts->entries; i++) {

        if (!strcmp(opts->defs->defs[i].name, name)) {

            if (opts->defs->defs[i].type != UTIL_OPTIONS_TYPE_STR) {
                log_warn("Option %s is not of type str", name);
                return 0;
            }

            if (opts->values[i].avail) {
                return opts->values[i].value.str;
            } else {
                return opts->defs->defs[i].default_value.str;
            }
        }
    }

    log_warn("Could not find str option %s", name);
    return NULL;
}

bool util_options_get_bool(const struct util_options_opts* opts,
        const char* name)
{
    for (uint32_t i = 0; i < opts->entries; i++) {

        if (!strcmp(opts->defs->defs[i].name, name)) {

            if (opts->defs->defs[i].type != UTIL_OPTIONS_TYPE_BOOL) {
                log_warn("Option %s is not of type bool", name);
                return false;
            }

            if (opts->values[i].avail) {
                return opts->values[i].value.b;
            } else {
                return opts->defs->defs[i].default_value.b;
            }
        }
    }

    log_warn("Could not find bool option %s", name);
    return false;
}

const uint8_t* util_options_get_bin(const struct util_options_opts* opts,
        const char* name, size_t* length)
{
    for (uint32_t i = 0; i < opts->entries; i++) {

        if (!strcmp(opts->defs->defs[i].name, name)) {

            if (opts->defs->defs[i].type != UTIL_OPTIONS_TYPE_BIN) {
                log_warn("Option %s is not of type bin", name);
                return 0;
            }

            if (opts->values[i].avail) {
                if (length) {
                    *length = opts->values[i].value.bin.len;
                }

                return opts->values[i].value.bin.data;
            } else {
                if (length) {
                    *length = opts->defs->defs[i].default_value.bin.len;
                }

                return opts->defs->defs[i].default_value.bin.data;
            }
        }
    }

    log_warn("Could not find bin option %s", name);
    return NULL;
}

void util_options_print_usage(const struct util_options_defs* option_defs)
{
    printf(
        "%s\nSpecify options as arguments or use --options [path file] to "
        "specify a config file containing key=value per line entries\n",
        option_defs->usage_header);

    for (uint32_t i = 0; i < option_defs->ndefs; i++) {

        switch (option_defs->defs[i].type) {
            case UTIL_OPTIONS_TYPE_INT:
            {
                printf(
                    "    -%c [int]   %s: %s\n"
                    "        default: %d\n",
                    option_defs->defs[i].param, option_defs->defs[i].name,
                    option_defs->defs[i].description,
                    option_defs->defs[i].default_value.i);

                break;
            }

            case UTIL_OPTIONS_TYPE_DOUBLE:
            {
                printf(
                    "    -%c [double]   %s: %s\n"
                    "        default: %f\n",
                    option_defs->defs[i].param, option_defs->defs[i].name,
                    option_defs->defs[i].description,
                    option_defs->defs[i].default_value.d);

                break;
            }

            case UTIL_OPTIONS_TYPE_STR:
            {
                printf(
                    "    -%c [str]   %s: %s\n"
                    "        default: %s\n",
                    option_defs->defs[i].param, option_defs->defs[i].name,
                    option_defs->defs[i].description,
                    option_defs->defs[i].default_value.str);

                break;
            }

            case UTIL_OPTIONS_TYPE_BOOL:
            {
                printf(
                    "    -%c [bool]  %s: %s\n"
                    "        default: %d\n",
                    option_defs->defs[i].param, option_defs->defs[i].name,
                    option_defs->defs[i].description,
                    option_defs->defs[i].default_value.b);

                break;
            }

            case UTIL_OPTIONS_TYPE_BIN:
            {
                size_t len = option_defs->defs[i].default_value.bin.len * 2 + 1;
                char* buf = util_xmalloc(len);

                util_hex_encode_uc(option_defs->defs[i].default_value.bin.data,
                    option_defs->defs[i].default_value.bin.len, buf, len);

                printf(
                    "    -%c [bin]  %s: %s\n"
                    "        default (len %Iu): %s\n",
                    option_defs->defs[i].param, option_defs->defs[i].name,
                    option_defs->defs[i].description,
                    option_defs->defs[i].default_value.bin.len,
                    buf);

                free(buf);

                break;
            }

            default:
            {
                printf(
                    "    -%c [unkn]  %s: %s\n"
                    "        default: -invalid-\n",
                    option_defs->defs[i].param, option_defs->defs[i].name,
                    option_defs->defs[i].description);

                break;
            }
        }
    }
}

void util_options_free(struct util_options_opts* opts)
{
    for (uint32_t i = 0; i < opts->entries; i++) {

        switch (opts->defs->defs[i].type) {
            case UTIL_OPTIONS_TYPE_STR:
            {
                if (opts->values[i].value.str) {
                    free(opts->values[i].value.str);
                }

                break;
            }

            case UTIL_OPTIONS_TYPE_BIN:
            {
                if (opts->values[i].value.bin.data) {
                    free(opts->values[i].value.bin.data);
                }

                break;
            }

            default:
                break;
        }
    }

    free(opts->values);
    free(opts);
}

// if NULL, usage is printed
static struct util_options_opts* util_options_get2(
        int argc, char** argv, const struct util_options_defs* option_defs)
{
    bool success;
    char* options_file = NULL;
    struct util_options_opts* options =
        (struct util_options_opts*)
            util_xmalloc(sizeof(struct util_options_opts));

    if (option_defs == NULL) {
        options->defs = NULL;
        options->values = NULL;
        options->entries = 0;

        return options;
    }

    options->defs = option_defs;
    options->entries = option_defs->ndefs;

    log_warn(">>>> %d", option_defs->ndefs);

    options->values = (struct util_options_value*) util_xmalloc(
        sizeof(struct util_options_value) * options->entries);

    memset(options->values, 0,
        sizeof(struct util_options_value) * options->entries);

    /* detect if we read options from a file */
    for (int i = 0; i < argc; i++) {

        if (!strcmp(argv[i], "--options")) {

            if (i + 1 >= argc) {
                log_warn("Missing argument for --options\n");

                util_options_print_usage(option_defs);
                util_options_free(options);
                return NULL;
            }

            options_file = argv[i + 1];
            break;
        }
    }

    if (options_file) {
        log_debug("Processing options file %s", options_file);
        success = util_options_init_from_file(options, option_defs,
            options_file);

        if (!success) {
            exit(0);
        }
    }

    log_debug("Processing cmd args");
    success = util_options_init_from_args(options, option_defs, argc, argv);

    if (!success) {
        util_options_print_usage(option_defs);
        util_options_free(options);
        return NULL;
    } else {
        return options;
    }
}

static bool util_options_init_from_args(struct util_options_opts* options,
        const struct util_options_defs* option_defs, int argc, char** argv)
{
    for (int i = 0; i < argc; i++) {

        if (argv[i][0] != '-') {
            continue;
        }

        for (uint32_t j = 0; j < options->entries; j++) {
            /* prints usage */
            if (argv[i][1] == option_defs->usage_param) {
                return false;
            }

            if (options->defs->defs[j].param == argv[i][1]) {

                switch (options->defs->defs[j].type) {
                    case UTIL_OPTIONS_TYPE_INT:
                    {
                        if (i + 1 >= argc) {
                            break;
                        }

                        ++i;

                        options->values[j].value.i = atoi(argv[i]);

                        break;
                    }

                    case UTIL_OPTIONS_TYPE_DOUBLE:
                    {
                        if (i + 1 >= argc) {
                            break;
                        }

                        ++i;

                        options->values[j].value.d = atof(argv[i]);

                        break;
                    }

                    case UTIL_OPTIONS_TYPE_STR:
                    {
                        if (i + 1 >= argc) {
                            break;
                        }

                        ++i;

                        /* FIXME memory leak if overriding string from options
                           file */

                        if (strlen(argv[i]) > 0) {
                            options->values[j].value.str = util_str_dup(argv[i]);
                        } else {
                            options->values[j].value.str = NULL;
                        }

                        break;
                    }

                    case UTIL_OPTIONS_TYPE_BOOL:
                    {
                        options->values[j].value.b = true;

                        break;
                    }

                    case UTIL_OPTIONS_TYPE_BIN:
                    {
                        if (i + 1 >= argc) {
                            break;
                        }

                        ++i;

                        /* FIXME memory leak if overriding bin data from options
                           file */

                       if (strlen(argv[i]) > 0) {
                            size_t len = strlen(argv[i]);
                            len = len / 2 + len % 2;

                            options->values[j].value.bin.len = len;
                            options->values[j].value.bin.data = util_xmalloc(len);

                            if (!util_hex_decode(options->values[j].value.bin.data,
                                    options->values[j].value.bin.len, argv[i],
                                    strlen(argv[i]))) {
                                log_warn("Decoding binary data for key %s "
                                    "failed", options->defs->defs[j].name);
                                options->values[j].value.bin.len = 0;
                                options->values[j].value.bin.data = NULL;
                            }
                        } else {
                            options->values[j].value.bin.len = 0;
                            options->values[j].value.bin.data = NULL;
                        }

                        break;
                    }

                    default:
                        continue;
                }

                log_debug("Cmd arg: -%c %s (type %d): %s",
                    options->defs->defs[j].param, options->defs->defs[j].name,
                    options->defs->defs[j].type, argv[i]);

                options->values[j].avail = true;
                break;
            }
        }
    }

    return true;
}



static bool util_options_init_from_file(struct util_options_opts* options,
        const struct util_options_defs* option_defs, char* file_path)
{
    char* pos_lines;
    char* pos_key_val;
    char* ctx_lines;
    char* ctx_key_val;
    char* data;
    size_t len;

    if (!util_file_load(file_path, (void**) &data, &len, true)) {
        /* If file does not exist, create one with default configuration
           values */
        if (util_fs_path_exists(file_path)) {
            log_warn("Loading options file %s failed, faulty or corrupted "
                "configuration", file_path);
        } else {
            /* Create a default config file if the usage param is set. This
               indicates that we are currently not on the inject process
               but already on the inject library. We want all config params
               of the inject lib */
            if (option_defs->usage_param) {
                log_info("Options file %s does not exist, creating default...",
                    file_path);

                if (!util_options_write_default_options_file(option_defs,
                        file_path)) {
                    log_warn("Creating default options file %s failed",
                        file_path);
                } else {
                    log_info("Options file %s created. Tweak it to your needs and restart the game.",
                        file_path);
                }
            }
        }

        return false;
    }

    pos_lines = strtok_r(data, "\n", &ctx_lines);

    while (pos_lines != NULL) {
        char* pos_line_dup;
        char* key = NULL;
        char* val = NULL;

        /* ignore comments and empty lines */
        if (strlen(pos_lines) > 0 && pos_lines[0] != '#') {
            pos_line_dup = util_str_dup(pos_lines);
            pos_key_val = strtok_r(pos_line_dup, "=", &ctx_key_val);

            log_debug("Line: %s", pos_lines);

            if (pos_key_val != NULL) {
                key = pos_key_val;
                val = &pos_key_val[strlen(key) + 1];
            } else {
                log_warn("Invalid options line %s in options file %s", pos_lines, file_path);
                free(pos_line_dup);
                free(data);
                return false;
            }

            log_debug("Key: %s, Value: %s", key, val);

            /* search for key */
            for (uint32_t j = 0; j < options->entries; j++) {

                if (!strcmp(options->defs->defs[j].name, key)) {

                    switch (options->defs->defs[j].type) {
                        case UTIL_OPTIONS_TYPE_INT:
                        {
                            if (val) {
                                options->values[j].value.i = atoi(val);
                            } else {
                                options->values[j].value.i = 0;
                            }

                            break;
                        }

                        case UTIL_OPTIONS_TYPE_DOUBLE:
                        {
                            if (val) {
                                options->values[j].value.d = atof(val);
                            } else {
                                options->values[j].value.d = 0.0;
                            }

                            break;
                        }

                        case UTIL_OPTIONS_TYPE_STR:
                        {
                            if (val && strlen(val) > 0) {
                                options->values[j].value.str =
                                    util_str_dup(val);
                            } else {
                                options->values[j].value.str = NULL;
                            }

                            break;
                        }

                        case UTIL_OPTIONS_TYPE_BOOL:
                        {
                            if (val) {
                                if (atoi(val) <= 0) {
                                    options->values[j].value.b = false;
                                } else {
                                    options->values[j].value.b = true;
                                }
                            } else {
                                options->values[j].value.b = false;
                            }


                            break;
                        }

                        case UTIL_OPTIONS_TYPE_BIN:
                        {
                            if (val && strlen(val) > 0) {
                                size_t lenBin = strlen(val);
                                lenBin = lenBin / 2 + lenBin % 2;

                                options->values[j].value.bin.len = lenBin;
                                options->values[j].value.bin.data =
                                    util_xmalloc(lenBin);

                                if (!util_hex_decode(
                                        options->values[j].value.bin.data,
                                        options->values[j].value.bin.len, val,
                                        strlen(val))) {
                                    log_warn("Decoding binary data for key "
                                        "%s failed",
                                        options->defs->defs[j].name);
                                    options->values[j].value.bin.len = 0;
                                    options->values[j].value.bin.data = NULL;
                                }
                            } else {
                                options->values[j].value.bin.len = 0;
                                options->values[j].value.bin.data = NULL;
                            }

                            break;
                        }

                        default:
                            continue;
                    }

                    log_debug("Options file param: -%c %s (type %d): %s",
                        options->defs->defs[j].param,
                        options->defs->defs[j].name,
                        options->defs->defs[j].type, val ? val : "NULL");

                    options->values[j].avail = true;
                    break;
                }
            }

            free(pos_line_dup);
        }

        pos_lines = strtok_r(NULL, "\n", &ctx_lines);
    }

    free(data);

    return true;
}

static bool util_options_write_default_options_file(
        const struct util_options_defs* option_defs, char* file_path)
{
    FILE* file;

    file = fopen(file_path, "wb");

    if (file == NULL) {
        return false;
    }

     for (uint32_t i = 0; i < option_defs->ndefs; i++) {

        switch (option_defs->defs[i].type) {
            case UTIL_OPTIONS_TYPE_INT:
            {
                fprintf(file, "# [int]: %s\n",
                    option_defs->defs[i].description);
                fprintf(file, "%s=%d\n\n", option_defs->defs[i].name,
                    option_defs->defs[i].default_value.i);

                break;
            }

            case UTIL_OPTIONS_TYPE_DOUBLE:
            {
                fprintf(file, "# [double]: %s\n",
                    option_defs->defs[i].description);
                fprintf(file, "%s=%f\n\n", option_defs->defs[i].name,
                    option_defs->defs[i].default_value.d);

                break;
            }

            case UTIL_OPTIONS_TYPE_STR:
            {
                fprintf(file, "# [str]: %s\n",
                    option_defs->defs[i].description);

                if (option_defs->defs[i].default_value.str) {
                    fprintf(file, "%s=%s\n\n", option_defs->defs[i].name,
                        option_defs->defs[i].default_value.str);
                } else {
                    fprintf(file, "%s=\n\n", option_defs->defs[i].name);
                }

                break;
            }

            case UTIL_OPTIONS_TYPE_BOOL:
            {
                fprintf(file, "# [bool (0/1)]: %s\n",
                    option_defs->defs[i].description);
                fprintf(file, "%s=%d\n\n", option_defs->defs[i].name,
                    option_defs->defs[i].default_value.b);

                break;
            }

            case UTIL_OPTIONS_TYPE_BIN:
            {
                fprintf(file, "# [bin]: %s\n",
                    option_defs->defs[i].description);

                if (option_defs->defs[i].default_value.bin.data) {
                    size_t len =
                        option_defs->defs[i].default_value.bin.len * 2 + 1;
                    char* buf = util_xmalloc(len);

                    util_hex_encode_uc(
                        option_defs->defs[i].default_value.bin.data,
                        option_defs->defs[i].default_value.bin.len, buf, len);


                    fprintf(file, "%s=%s\n\n", option_defs->defs[i].name, buf);

                    free(buf);
                }

                break;
            }

            default:
            {
                log_warn("Invalid value type %d found",
                    option_defs->defs[i].type);
                break;
            }
        }
    }

    fclose(file);
    return true;
}