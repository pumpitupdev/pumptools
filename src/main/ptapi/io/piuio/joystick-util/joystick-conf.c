#include "ptapi/io/piuio/joystick-util/joystick-conf.h"

#include "util/fs.h"
#include "util/log.h"
#include "util/mem.h"

const char* ptapi_io_piuio_joystick_util_conf_input_str[PTAPI_IO_PIUIO_JOYSTICK_UTIL_CONF_INPUT_COUNT] = {
    "invalid",
    "P1 Left Up",
    "P1 Right Up",
    "P1 Center",
    "P1 Left Down",
    "P1 Right Down",
    "P2 Left Up",
    "P2 Right Up",
    "P2 Center",
    "P2 Left Down",
    "P2 Right Down",
    "Service",
    "Test",
    "Coin Clear",
    "Coin",
    "Coin 2"
};

void ptapi_io_piuio_joystick_util_conf_new(struct ptapi_io_piuio_joystick_util_conf** conf)
{
    log_assert(conf);

    *conf = util_xmalloc(sizeof(struct ptapi_io_piuio_joystick_util_conf));
    (*conf)->num_entries = 0;
}

bool ptapi_io_piuio_joystick_util_conf_read_from_file(struct ptapi_io_piuio_joystick_util_conf** conf, const char* path)
{
    log_assert(path);
    log_assert(conf);

    void* buffer;
    size_t size;

    if (!util_file_load(path, &buffer, &size, false)) {
        return false;
    }

    *conf = (struct ptapi_io_piuio_joystick_util_conf*) buffer;

    return true;
}

bool ptapi_io_piuio_joystick_util_conf_write_to_file(struct ptapi_io_piuio_joystick_util_conf* conf, const char* path)
{
    log_assert(path);
    log_assert(conf);

    return util_file_save(path, conf, sizeof(struct ptapi_io_piuio_joystick_util_conf) +
        sizeof(struct ptapi_io_piuio_joystick_util_conf_entry) * conf->num_entries);
}

bool ptapi_io_piuio_joystick_util_conf_set_entry(struct ptapi_io_piuio_joystick_util_conf* conf,
        struct ptapi_io_piuio_joystick_util_conf_entry* entry)
{
    log_assert(conf);
    log_assert(entry);

    for (size_t i = 0; i < conf->num_entries; i++) {
        if (!strcmp(conf->entries[i].name, entry->name) && !strcmp(conf->entries[i].dev_path, entry->dev_path)) {
            memcpy(&conf->entries[i], entry, sizeof(struct ptapi_io_piuio_joystick_util_conf_entry));
            return true;
        }
    }

    return false;
}

void ptapi_io_piuio_joystick_util_conf_append_entry(struct ptapi_io_piuio_joystick_util_conf** conf,
        struct ptapi_io_piuio_joystick_util_conf_entry* entry)
{
    log_assert(conf);
    log_assert(entry);

    (*conf)->num_entries++;

    *conf = util_xrealloc(*conf, sizeof(struct ptapi_io_piuio_joystick_util_conf*) +
        sizeof(struct ptapi_io_piuio_joystick_util_conf_entry) * (*conf)->num_entries);

    memcpy(&(*conf)->entries[(*conf)->num_entries - 1], entry, sizeof(struct ptapi_io_piuio_joystick_util_conf_entry));
};

bool ptapi_io_piuio_joystick_util_conf_find_entry(struct ptapi_io_piuio_joystick_util_conf* conf, const char* name,
        const char* dev_path, struct ptapi_io_piuio_joystick_util_conf_entry* entry)
{
    log_assert(name);
    log_assert(dev_path);
    log_assert(entry);

    for (size_t i = 0; i < conf->num_entries; i++) {
        if (!strcmp(conf->entries[i].name, name) && !strcmp(conf->entries[i].dev_path, dev_path)) {
            memcpy(entry, &conf->entries[i], sizeof(struct ptapi_io_piuio_joystick_util_conf_entry));

            return true;
        }
    }

    return false;
}