/**
 * Tool for NX2 profiles: decrypt, encrypt, print profile data
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "asset/nx2/lib/util.h"
#include "asset/nx2/lib/usb-save.h"

#include "util/fs.h"
#include "util/str.h"

int main(int argc, char **argv)
{
  int ret = 0;
  char *rank_path_encrypted;
  char *save_path_encrypted;
  char *rank_path_decrypted;
  char *save_path_decrypted;

  struct asset_nx2_usb_rank *rank = NULL;
  struct asset_nx2_usb_save *save = NULL;

  if (argc < 2 || argc > 3)
  {
    printf("Usage: %s help\n", argv[0]);
    return -1;
  }

  if (!strcmp(argv[1], "help"))
  {
    printf(
        "Usage: %s [COMMAND] <Option> (Directory)\n\n"
        "Commands:\n"
        "  [new] - Create new default profile files\n"
        "  [dec] - Decrypt existing profile files\n"
        "  [enc] - Encrypt existing decrypted profile files\n"
        "  [dump] - Print existing profile files to txt\n"
        "  [edit] - Edit player name, avatar, and mileage in profile files\n"
        "  [help] - Show this help message\n\n"
        "(Directory) - Directory containing nx2rank.bin.dec nx2save.bin.dec\n"
        "\n",
        argv[0]);
    return -1;
  }

  if (argc == 3)
  {
    const char *base = argv[argc - 1];
    size_t blen = strlen(base);
    int has_slash = (blen > 0 && base[blen - 1] == '/');

    if (has_slash) {
      rank_path_encrypted = util_str_merge(base, "nx2rank.bin");
      save_path_encrypted = util_str_merge(base, "nx2save.bin");
      rank_path_decrypted = util_str_merge(base, "nx2rank.dec");
      save_path_decrypted = util_str_merge(base, "nx2save.dec");
    } else {
      rank_path_encrypted = util_str_merge(base, "/nx2rank.bin");
      save_path_encrypted = util_str_merge(base, "/nx2save.bin");
      rank_path_decrypted = util_str_merge(base, "/nx2rank.dec");
      save_path_decrypted = util_str_merge(base, "/nx2save.dec");
    }
  }
  else
  {
    rank_path_encrypted = strdup("nx2rank.bin");
    save_path_encrypted = strdup("nx2save.bin");
    rank_path_decrypted = strdup("nx2rank.dec");
    save_path_decrypted = strdup("nx2save.dec");
  }

  if(!strcmp(argv[1], "new"))
  {

    struct asset_nx2_usb_rank *rank = asset_nx2_usb_rank_new();
    struct asset_nx2_usb_save *save = asset_nx2_usb_save_new();

    asset_nx2_usb_rank_finalize(rank);
    asset_nx2_usb_save_finalize(save);

    if (!asset_nx2_usb_rank_save_to_file(rank_path_encrypted, rank, true)) {
      fprintf(stderr, "Creating rank file %s failed\n", rank_path_encrypted);
      ret = -2;
    }

    if (!asset_nx2_usb_save_save_to_file(save_path_encrypted, save, true)) {
      fprintf(stderr, "Creating save file %s failed\n", save_path_encrypted);
      ret = -3;
    }

    free(rank);
    free(save);

  } else if (!strcmp(argv[1], "dec"))
    {
  
      rank = asset_nx2_usb_rank_load_from_file(rank_path_encrypted, true);
      save = asset_nx2_usb_save_load_from_file(save_path_encrypted, true);
  
      if (!rank) {
        fprintf(stderr, "Loading %s failed\n", rank_path_encrypted);
        ret = -4;
      } else if (!save) {
        fprintf(stderr, "Loading %s failed\n", save_path_encrypted);
        ret = -5;
        free(rank);
      } else {
  
        if (!asset_nx2_usb_rank_save_to_file(rank_path_decrypted, rank, false)) {
          fprintf(stderr, "Saving decrypted rank file to %s failed\n", rank_path_decrypted);
          ret = -6;
        }
  
        if (!asset_nx2_usb_save_save_to_file(save_path_decrypted, save, false)) {
          fprintf(stderr, "Saving decrypted save file to %s failed\n", save_path_decrypted);
          ret = -7;
        }

        ret = 0;
      }
    }
    else if (!strcmp(argv[1], "enc"))
    {
    
      rank = asset_nx2_usb_rank_load_from_file(rank_path_decrypted, false);
      save = asset_nx2_usb_save_load_from_file(save_path_decrypted, false);
      
      {
        if (util_fs_path_exists(rank_path_encrypted)) {
          char *rank_old = util_str_merge(rank_path_encrypted, "_old");
          if (rename(rank_path_encrypted, rank_old) != 0) {
            fprintf(stderr, "Renaming %s to %s failed\n", rank_path_encrypted, rank_old);
          }
          free(rank_old);
        }

        if (util_fs_path_exists(save_path_encrypted)) {
          char *save_old = util_str_merge(save_path_encrypted, "_old");
          if (rename(save_path_encrypted, save_old) != 0) {
            fprintf(stderr, "Renaming %s to %s failed\n", save_path_encrypted, save_old);
          }
          free(save_old);
        }
      }

      if (!asset_nx2_usb_rank_save_to_file(rank_path_encrypted, rank, true)) {
        fprintf(
            stderr, "Saving encrypted rank file to %s failed\n", rank_path_encrypted);
        ret = -6;
      }

      if (!asset_nx2_usb_save_save_to_file(save_path_encrypted, save, true)) {
        fprintf(
            stderr, "Saving encrypted save file to %s failed\n", save_path_encrypted);
        ret = -7;
      }
  
      ret = 0;
    }
    else if (!strcmp(argv[1], "dump"))
    {
      rank = asset_nx2_usb_rank_load_from_file(rank_path_decrypted, false);
      save = asset_nx2_usb_save_load_from_file(save_path_decrypted, false);
      
      if (!rank) {
        fprintf(stderr, "Loading %s failed\n", rank_path_decrypted);
        ret = -4;
        free(save);
        goto cleanup;
      }
      if (!save) {
        fprintf(stderr, "Loading %s failed\n", save_path_encrypted);
        ret = -5;
        free(rank);
        goto cleanup;
      }
      
      char *rank_str;
      char *save_str;
      rank_str = asset_nx2_usb_rank_to_string(rank);
      save_str = asset_nx2_usb_save_to_string(save);

      char *dump_path = util_str_merge(rank_path_decrypted, ".dump.txt");
      
      char *header1 = "----------------- rank -----------------\n";
      char *header2 = "----------------- save -----------------\n";
      
      size_t total_len = strlen(header1) + strlen(rank_str) + strlen(header2) + strlen(save_str) + 1;
      char *dump_content = malloc(total_len);
      
      if (!dump_content) {
        fprintf(stderr, "Memory allocation failed for dump content\n");
        ret = -8;
      } else {
        snprintf(dump_content, total_len, "%s%s%s%s", header1, rank_str, header2, save_str);
        
        if (!util_file_save(dump_path, dump_content, strlen(dump_content))) {
          fprintf(stderr, "Failed to save dump file %s\n", dump_path);
          ret = -8;
        } else {
          printf("Profile data dumped to %s\n", dump_path);
        }
        
        free(dump_content);
      }

      free(dump_path);
      free(rank_str);
      free(save_str);

      ret = 0;
    } else if(!strcmp(argv[1], "edit")) 
    {

      rank = asset_nx2_usb_rank_load_from_file(rank_path_encrypted, true);
      save = asset_nx2_usb_save_load_from_file(save_path_encrypted, true);

      if (!rank) {
        fprintf(stderr, "Loading %s failed\n", rank_path_encrypted);
        ret = -4;
        free(save);
        goto cleanup;
      }
      if (!save) {
        fprintf(stderr, "Loading %s failed\n", save_path_encrypted);
        ret = -5;
        free(rank);
        goto cleanup;
      }

      printf("Current player name: %s\n", save->review.player_id);
      printf("Enter new player name (max 8 characters, A-Z 0-9 only, leave blank to keep current): ");
      char new_player_name[9];
      fgets(new_player_name, sizeof(new_player_name), stdin);
      new_player_name[strcspn(new_player_name, "\n")] = 0;

      size_t len = strlen(new_player_name);
      if (len > 8) {
        fprintf(stderr, "Error: Player name must be 8 characters or less\n");
        ret = -9;
        goto cleanup;
      }

      if (len > 0) {
        for (size_t i = 0; i < len; i++) {
          if (new_player_name[i] >= 'a' && new_player_name[i] <= 'z') {
            new_player_name[i] = new_player_name[i] - 'a' + 'A';
          } else if (!((new_player_name[i] >= 'A' && new_player_name[i] <= 'Z') || 
                       (new_player_name[i] >= '0' && new_player_name[i] <= '9'))) {
            fprintf(stderr, "Error: Player name must contain only letters (A-Z) and numbers (0-9)\n");
            ret = -9;
            goto cleanup;
          }
        }


        strncpy(save->review.player_id, new_player_name, sizeof(save->review.player_id) - 1);
        save->review.player_id[sizeof(save->review.player_id) - 1] = 0;
        strncpy(save->stats.player_id, new_player_name, sizeof(save->stats.player_id) - 1);
        save->stats.player_id[sizeof(save->stats.player_id) - 1] = 0;
        

        for (int i = 0; i < ASSET_NX2_USB_SAVE_SONG_MAX; i++) {
          for (int j = 0; j < ASSET_NX2_USB_SAVE_NUM_MODES; j++) {

            if (save->stats.song_scores[i][j].player_id[0] != '\0') {
              strncpy(save->stats.song_scores[i][j].player_id, new_player_name, 
                      sizeof(save->stats.song_scores[i][j].player_id) - 1);
              save->stats.song_scores[i][j].player_id[sizeof(save->stats.song_scores[i][j].player_id) - 1] = 0;
            }
          }
        }
        
        printf("Player name updated to: %s\n", save->review.player_id);
      } else {
        printf("Player name kept as: %s\n", save->review.player_id);
      }

      printf("Current profile picture: %d\n", save->stats.avatar_id);
      printf("Enter new profile picture number (0-127, leave blank to keep current): ");
      char pic_buf[16];
      if (!fgets(pic_buf, sizeof(pic_buf), stdin)) {
        fprintf(stderr, "Error: Failed to read input\n");
        ret = -9;
        goto cleanup;
      }
      
      pic_buf[strcspn(pic_buf, "\n")] = 0;
      
      if (strlen(pic_buf) > 0) {
        char *endp = NULL;
        long pic_val = strtol(pic_buf, &endp, 10);
        if (endp == pic_buf || *endp != '\0' || pic_val < 0 || pic_val > 127) {
          fprintf(stderr, "Error: Picture number must be an integer between 0 and 127\n");
          ret = -9;
          goto cleanup;
        }
        
        save->stats.avatar_id = (unsigned char)pic_val;
        printf("Profile picture updated to: %ld\n", pic_val);
      } else {
        printf("Profile picture kept as: %d\n", save->stats.avatar_id);
      }

      printf("Current mileage: %d\n", save->stats.mileage);
      printf("Enter new mileage (leave blank to keep current): ");
      char mileage_buf[16];
      if (!fgets(mileage_buf, sizeof(mileage_buf), stdin)) {
        fprintf(stderr, "Error: Failed to read input\n");
        ret = -9;
        goto cleanup;
      }
      
      mileage_buf[strcspn(mileage_buf, "\n")] = 0;
      
      if (strlen(mileage_buf) > 0) {
        char *endp = NULL;
        long mileage_val = strtol(mileage_buf, &endp, 10);
        if (endp == mileage_buf || *endp != '\0' || mileage_val < 0 || mileage_val > INT32_MAX) {
          fprintf(stderr, "Error: Mileage must be a positive integer\n");
          ret = -9;
          goto cleanup;
        }
        

        int32_t old_mileage = save->stats.mileage;
        save->review.mileage = (int32_t)mileage_val;
        save->stats.mileage = (int32_t)mileage_val;
        printf("Mileage updated from %d to %ld\n", old_mileage, mileage_val);
      } else {
        printf("Mileage kept as: %d\n", save->stats.mileage);
      }

      asset_nx2_usb_save_finalize(save);


      if (!asset_nx2_usb_rank_save_to_file(rank_path_encrypted, rank, true)) {
        fprintf(stderr, "Failed to save updated rank file %s\n", rank_path_encrypted);
        ret = -6;
      }

      if (!asset_nx2_usb_save_save_to_file(save_path_encrypted, save, true)) {
        fprintf(stderr, "Failed to save updated save file %s\n", save_path_encrypted);
        ret = -7;
      }

      if (ret == 0) {
        printf("Profile files updated successfully!\n");
      }

      ret = 0;
    } else {
      printf("Unknown command. For usage: %s help\n", argv[0]);
      
      ret = 0;
    }

cleanup:
    if (rank) free(rank);
    if (save) free(save);
  
  free(rank_path_encrypted);
  free(save_path_encrypted);
  free(rank_path_decrypted);
  free(save_path_decrypted);

  return ret;
}

