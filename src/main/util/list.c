#include "util/list.h"

#include <stddef.h>
#include <string.h>

void util_list_init(struct util_list *list)
{
  memset(list, 0, sizeof(*list));
}

void util_list_append(struct util_list *list, struct util_list_node *node)
{
  node->next = NULL;

  if (list->tail != NULL) {
    list->tail->next = node;
  } else {
    list->head = node;
  }

  list->tail = node;
}

bool util_list_contains(struct util_list *list, struct util_list_node *node)
{
  struct util_list_node *pos;

  for (pos = list->head; pos != NULL; pos = pos->next) {
    if (pos == node) {
      return true;
    }
  }

  return false;
}

struct util_list_node *util_list_pop_head(struct util_list *list)
{
  struct util_list_node *node;

  if (list->head == NULL) {
    return NULL;
  }

  node = list->head;
  list->head = node->next;

  if (node->next == NULL) {
    list->tail = NULL;
  }

  node->next = NULL;

  return node;
}

void util_list_remove(struct util_list *list, struct util_list_node *node)
{
  struct util_list_node *pos;
  struct util_list_node *prev;

  for (prev = NULL, pos = list->head; pos != NULL;
       prev = pos, pos = pos->next) {
    if (pos == node) {
      if (prev != NULL) {
        prev->next = node->next;
      } else {
        list->head = node->next;
      }

      if (node->next == NULL) {
        list->tail = prev;
      }

      node->next = NULL;

      return;
    }
  }
}

bool util_list_empty(struct util_list *list)
{
  return list->head == NULL;
}

inline struct util_list_node *util_list_peek_head(struct util_list *list)
{
  return list->head;
}
