/**
 * Tool for FiestaEX profiles: decrypt, encrypt, print profile data
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asset/fex/lib/util.h"

#include "util/str.h"

int main(int argc, char** argv)
{
    int ret;
    char* rank_path;
    char* save_path;

    if (argc < 3) {
        printf("Usage: %s [cmd: new, dec, enc, dump] "
            "[path containing fiestaex_rank.bin fiestaex_save.bin]\n", argv[0]);
        return -1;
    }

    ret = 0;

    rank_path = util_str_merge(argv[2], "fiestaex_rank.bin");
    save_path = util_str_merge(argv[2], "fiestaex_save.bin");

    if (!strcmp(argv[1], "new")) {
        struct asset_fex_usb_rank* rank = asset_fex_usb_rank_new();
        struct asset_fex_usb_save* save = asset_fex_usb_save_new();

        asset_fex_usb_rank_finalize(rank);
        asset_fex_usb_save_finalize(save);

        if (!asset_fex_usb_rank_save_to_file(rank_path, rank, true)) {
            fprintf(stderr, "Creating rank file %s failed\n", rank_path);
            ret = -2;
        }

        if (!asset_fex_usb_save_save_to_file(save_path, save, true)) {
            fprintf(stderr, "Creating save file %s failed\n", save_path);
            ret = -3;
        }

        free(rank);
        free(save);
    } else {
        struct asset_fex_usb_rank* rank;
        struct asset_fex_usb_save* save;
        bool encrypted;

        if (!strcmp(argv[1], "enc")) {
            char* tmp;

            encrypted = false;

            tmp = util_str_merge(rank_path, ".dec");
            free(rank_path);
            rank_path = tmp;

            tmp = util_str_merge(save_path, ".dec");
            free(save_path);
            save_path = tmp;
        } else {
            encrypted = true;
        }

        rank = asset_fex_usb_rank_load_from_file(rank_path, encrypted);
        save = asset_fex_usb_save_load_from_file(save_path, encrypted);

        if (!rank) {
            fprintf(stderr, "Loading %s failed", rank_path);
            ret = -4;
            goto cleanup;
        }

        if (!save) {
            fprintf(stderr, "Loading %s failed", save_path);
            ret = -5;
            free(rank);
            goto cleanup;
        }

        if (!strcmp(argv[1], "dec")) {
            char* rank_path_dec;
            char* save_path_dec;

            rank_path_dec = util_str_merge(rank_path, ".dec");
            save_path_dec = util_str_merge(save_path, ".dec");

            if (!asset_fex_usb_rank_save_to_file(rank_path_dec, rank, false)) {
                fprintf(stderr, "Saving decrypted rank file to %s failed\n",
                    rank_path_dec);
                ret = -6;
            }

            if (!asset_fex_usb_save_save_to_file(save_path_dec, save, false)) {
                fprintf(stderr, "Saving decrypted save file to %s failed\n",
                    save_path_dec);
                ret = -7;
            }

            free(rank_path_dec);
            free(save_path_dec);
        } else if (!strcmp(argv[1], "enc")) {
            char* rank_path_enc;
            char* save_path_enc;

            rank_path_enc = util_str_merge(rank_path, ".enc");
            save_path_enc = util_str_merge(save_path, ".enc");

            if (!asset_fex_usb_rank_save_to_file(rank_path_enc, rank, true)) {
                fprintf(stderr, "Saving encrypted rank file to %s failed\n",
                    rank_path_enc);
                ret = -6;
            }

            if (!asset_fex_usb_save_save_to_file(save_path_enc, save, true)) {
                fprintf(stderr, "Saving encrypted save file to %s failed\n",
                    save_path_enc);
                ret = -7;
            }

            free(rank_path_enc);
            free(save_path_enc);
        } else if (!strcmp(argv[1], "dump")) {
            char* rank_str;
            char* save_str;

            rank_str = asset_fex_usb_rank_to_string(rank);
            save_str = asset_fex_usb_save_to_string(save);

            printf("----------------- rank -----------------\n"
                "%s----------------- save -----------------\n%s",
                rank_str, save_str);

            free(rank_str);
            free(save_str);
        } else {
            fprintf(stderr, "Unknown command %s\n", argv[1]);
            ret = -4;
        }

        free(rank);
        free(save);
    }

cleanup:
    free(rank_path);
    free(save_path);

    return ret;
}