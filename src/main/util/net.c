#include <stdlib.h>
#include <string.h>

#include "util/net.h"
#include "util/str.h"

bool util_net_split_ipv4_str(const char *str, char **address, uint16_t *port)
{
  char *tmp = util_str_dup(str);
  char *pch = strtok(tmp, ":");
  int counter = 0;

  *address = NULL;
  *port = 0xFFFF;

  while (pch != NULL) {
    switch (counter) {
      case 0:
        *address = util_str_dup(pch);
        break;

      case 1:
        *port = (uint16_t) strtol(pch, NULL, 10);
        break;

      default:
        if (*address) {
          free(*address);
        }

        free(tmp);
        return false;
    }

    pch = strtok(NULL, ":");
    counter++;
  }

  free(tmp);
  return true;
}