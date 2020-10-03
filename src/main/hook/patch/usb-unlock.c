#define LOG_MODULE "patch-usb-unlock"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "capnhook/hook/lib.h"

#include "util/log.h"

#include "usb-unlock.h"

#define SERIAL_UNLOCK_TOKEN "PUMPToken\0"

typedef int (*sscanf_t)(const char *str, const char *format, ...);
typedef int (*strcasecmp_t)(const char *s1, const char *s2);

static sscanf_t patch_usb_unlock_real_sscanf;
static strcasecmp_t patch_usb_unlock_real_strcasecmp;

int sscanf(const char *str, const char *format, ...)
{
    if (!patch_usb_unlock_real_sscanf) {
        patch_usb_unlock_real_sscanf = (sscanf_t) cnh_lib_get_func_addr("sscanf");
    }

	va_list ap;
	va_start(ap, format);

    if (!strcmp(format, "P:  Vendor=%4x ProdID=%4x")) {
        log_debug("Spoofing VID 0x058f for usb stick");

		/* unpack argument list, we know we got 2 arguments here */
		/* VID */
        *va_arg(ap, int*) = 0x058f;
        /* PID */
		*va_arg(ap, int*) = 0;

        va_end(ap);
        return 2;
    }

    if (!strcmp(format, "S:  SerialNumber=%s")) {
        log_debug("Spoofing serial number of usb stick to kill serial number locking of profile in a moment...");
        memcpy(va_arg(ap, char*), SERIAL_UNLOCK_TOKEN, strlen(SERIAL_UNLOCK_TOKEN) + 1);

        va_end(ap);
        return 1;
    }

    int ret = vsscanf(str, format, ap);
	va_end(ap);
	return ret;
}

int strcasecmp(const char *s1, const char *s2)
{
    if (!patch_usb_unlock_real_strcasecmp) {
        patch_usb_unlock_real_strcasecmp = (strcasecmp_t) cnh_lib_get_func_addr("strcasecmp");
    }

    if (!strcmp(s2, SERIAL_UNLOCK_TOKEN)) {
        log_debug("Caught serial unlock token...unlocking profile");
        return 0;
    }

	return patch_usb_unlock_real_strcasecmp(s1, s2);
}

void patch_usb_unlock_init(void)
{
    log_info("Initialized");
}