#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "ptapi/io/piuio/keyboard-util/keyboard-conf.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/time.h"

#define CONFIG_FILENAME "piuio-keyboard-conf.bin"

static Display* display;
static Window window;

static uint32_t keyboard_get_next_input()
{
    XEvent event;
    KeySym key;

    key = 0;

    while (true) {
        XNextEvent(display, &event);

        if (event.type == KeyPress) {
            XLookupString(&event.xkey, 0, 0, &key, 0);
        } else if (event.type == KeyRelease) {
            break;
        }
    }

    return key;
}

static void menu_keyboard_test()
{
    printf("Pressed + released inputs:\n");

    while (true) {
        printf("%d\n", keyboard_get_next_input());
    }
}

static void menu_keyboard_map()
{
    struct ptapi_io_piuio_keyboard_util_conf* config;

    if (!ptapi_io_piuio_keyboard_util_conf_read_from_file(&config, CONFIG_FILENAME)) {
        printf("Reading existing config file failed, creating new\n");

        ptapi_io_piuio_keyboard_util_conf_new(&config);
    } else {
        printf("Found existing mapping entry in config for keyboard, overwriting\n");
    }

    printf("A new window with no contents/white background is opened. Ensure to have the application focus on that "
           "window to allow input capturing. Otherwise, the tool cannot capture any keyboard inputs!\n");

    // Delay a moment to avoid picking up any previous keyboard inputs
    util_time_sleep_ms(500);

    for (uint32_t i = PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_P1_LU; i < PTAPI_IO_PIUIO_KEYBOARD_UTIL_CONF_INPUT_COUNT;
            i++) {
        printf("Press button to assign to \"%s\" or 'Enter/Return' to skip: ",
            ptapi_io_piuio_keyboard_util_conf_input_str[i]);
        fflush(stdout);

        config->button_map[i] = keyboard_get_next_input();

        if (config->button_map[i] == XK_Return) {
            config->button_map[i] = 0;
            printf("SKIP\n");
        } else {
            printf("%d\n", config->button_map[i]);
        }
    }

    ptapi_io_piuio_keyboard_util_conf_write_to_file(config, CONFIG_FILENAME);

    free(config);

    printf("Configuration file saved: %s\n", CONFIG_FILENAME);
}

int main(int argc, char** argv)
{
    bool main_loop;
    int screen;

    util_log_set_file("keyboard-conf.log", false);
    util_log_set_level(LOG_LEVEL_ERROR);

    display = XOpenDisplay(NULL);

    if (!display) {
        printf("Opening display failed\n");
        return -1;
    }

    screen = DefaultScreen(display);

    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 200, 200, 1,
        BlackPixel(display, screen), WhitePixel(display, screen));

    XSelectInput(display, window, KeyPressMask | KeyReleaseMask);
    XMapWindow(display, window);

    printf("ptapi PIUIO keyboard button mapping config tool\n");

    main_loop = true;

    while (main_loop) {
        uint32_t item;

        printf("Main menu, please select:\n");
        printf("1 > Keyboard input test\n");
        printf("2 > Configure keyboard mappings\n");
        printf("3 > Exit\n");
        printf("Select number: ");

        scanf("%d", &item);

        switch (item) {
            case 1:
                menu_keyboard_test();
                break;

            case 2:
                menu_keyboard_map();
                break;

            case 3:
                main_loop = false;
                break;

            default:
                printf("Invalid item chosen: %d\n", item);
                break;
        }
    }

    XCloseDisplay(display);

    return 0;
}