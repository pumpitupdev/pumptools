#define LOG_MODULE "exchook-eeprom"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "hook/exc/mempatch.h"

#include "util/fs.h"
#include "util/log.h"
#include "util/patch.h"

static int32_t exchook_eeprom_read(uint8_t* buffer, int32_t len);
static int32_t exchook_eeprom_write(const uint8_t* buffer, int32_t len);

static char* exchook_eeprom_file;
static uint8_t exchook_eeprom_buffer[8192];

void exchook_eeprom_init(const struct exchook_mempatch_table* patch_table, const char* eeprom_file)
{
    void* buffer;
    size_t len;

    exchook_eeprom_file = util_fs_get_abs_path(eeprom_file);

    /* EEPROM file does not exist, create new */
    if (!exchook_eeprom_file) {
        memset(exchook_eeprom_buffer, 0xFF, sizeof(exchook_eeprom_buffer));

        FILE* file = fopen(eeprom_file, "wb+");

        if (file == NULL) {
            log_error("Creating new eeprom file %s failed: %s", eeprom_file, strerror(errno));
        }

        fwrite(exchook_eeprom_buffer, sizeof(exchook_eeprom_buffer), 1, file);
        fclose(file);

        exchook_eeprom_file = util_fs_get_abs_path(eeprom_file);
        len = (size_t) -1;
    } else {
        if (util_file_load(exchook_eeprom_file, &buffer, &len, false)) {
            if (len > sizeof(exchook_eeprom_buffer)) {
                log_error("Contents of %s eeprom file exceed the max eeprom buffer "
                    " size of %d bytes", eeprom_file,
                    sizeof(exchook_eeprom_buffer));
                free(buffer);
                return;
            }

            /* copy read contents */
            memcpy(exchook_eeprom_buffer, buffer, len);
            free(buffer);
        } else {
            log_error("Loading eeprom file %s failed", exchook_eeprom_file);
        }
    }

    log_assert(exchook_eeprom_file);

    /* Hook eeprom */
    util_patch_function(patch_table->addr_eeprom_read, exchook_eeprom_read);
    util_patch_function(patch_table->addr_eeprom_write, exchook_eeprom_write);

    log_info("Initizlied (%s), eeprom file %s", len == -1 ? "new" : "loaded", eeprom_file);
}

static int32_t exchook_eeprom_read(uint8_t* buffer, int32_t len)
{
    if (len > sizeof(exchook_eeprom_buffer)) {
        log_error("Eeprom read %d exceeds buffer size %d", len,
            sizeof(exchook_eeprom_buffer));
        return 0;
    }

    memcpy(buffer, exchook_eeprom_buffer, len);

    return len;
}

static int32_t exchook_eeprom_write(const uint8_t* buffer, int32_t len)
{
    if (len > sizeof(exchook_eeprom_buffer)) {
        log_error("Eeprom write %d exceeds buffer size %d", len,
            sizeof(exchook_eeprom_buffer));
        return 0;
    }

    memcpy(exchook_eeprom_buffer, buffer, len);

    /* Write back to file */
    if (!util_file_save(exchook_eeprom_file, exchook_eeprom_buffer, len)) {
        log_error("Storing eeprom contents to file %s failed",
            exchook_eeprom_file);
    }

    return len;
}