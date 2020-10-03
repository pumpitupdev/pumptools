/**
 * Tool to test implementations of the piubtn API
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ptapi/io/piubtn.h"
#include "ptapi/io/piubtn/util/lib.h"

#include "util/log.h"

static struct ptapi_io_piubtn_api api;

int main(int argc, char** argv)
{
    struct ptapi_io_piubtn_inputs inputs[2];
    struct ptapi_io_piubtn_outputs outputs[2];

    util_log_set_file("piubtn-test.log", false);
    util_log_set_level(LOG_LEVEL_ERROR);

    if (argc < 2) {
        printf(
            "Usage: %s <piubtn.so> [debug]\n",
                argv[0]);
        return -1;
    }

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "debug")) {
            util_log_set_level(LOG_LEVEL_DEBUG);
            break;
        }
    }

    if (!ptapi_io_piubtn_util_lib_load(argv[1], &api)) {
        printf("Loading piubtn lib %s failed\n", argv[1]);
        return -2;
    }

    if (!api.open()) {
        return -3;
    }

    for (uint8_t i = 0; i < 2; i++) {
        memset(&inputs, 0, sizeof(inputs));
        memset(&outputs, 0, sizeof(outputs));
    }

    printf("Enter main loop by pressing any key (on the keyboard)\n");
    getchar();

    while (true) {
        if (!api.recv()) {
            return -4;
        }

        for (uint8_t i = 0; i < 2; i++) {
            api.get_input(i, &inputs[i]);
        }

        if (inputs[0].start && inputs[1].start) {
            break;
        }

        system("clear");

        printf("Press p1 start + p2 start to exit\n");
        printf(
            "|-------------------|\n"
            "|  %d             %d  |\n"
            "|%d   %d         %d   %d|\n"
            "|  %d             %d  |\n"
            "|-------------------|\n",
            inputs[0].back, inputs[1].back,
            inputs[0].left, inputs[0].right, inputs[1].left, inputs[1].right,
            inputs[0].start, inputs[1].start);

        for (int i = 0; i < 2; i++) {
            memset(&outputs[i], 0, sizeof(outputs[i]));

            inputs[i].start = outputs[i].start;
            inputs[i].back = outputs[i].back;
            inputs[i].left = outputs[i].left;
            inputs[i].right = outputs[i].right;

            api.set_output(i, &outputs[i]);
        }

        if (!api.send()) {
            return -5;
        }

        /* Avoid hardware banging */
        usleep(1000);
    }

    api.close();

    return 0;
}