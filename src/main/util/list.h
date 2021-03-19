#ifndef UTIL_LIST_H
#define UTIL_LIST_H

#include <stdbool.h>
#include <stddef.h>

/**
 * (Doubly linked) list object
 */
struct util_list {
  struct util_list_node *head;
  struct util_list_node *tail;
};

/**
 * List element/node
 */
struct util_list_node {
  struct util_list_node *next;
};

/**
 * Initialize an allocated list
 *
 * @param list Pointer to a list to initialize
 */
void util_list_init(struct util_list *list);

/**
 * Append an element to a list
 *
 * @param list List to append to
 * @param node Node to append to the list
 */
void util_list_append(struct util_list *list, struct util_list_node *node);

/**
 * Check if the list contains the specified element
 *
 * @param list List to check
 * @param node Node to look for in the list
 * @return True if list contains the specified node, false otherwise
 */
bool util_list_contains(struct util_list *list, struct util_list_node *node);

/**
 * Peak (get but do not remove) the head of the list
 *
 * @param list List to peak
 * @return Head element of the list (or null if empty)
 */
struct util_list_node *util_list_peek_head(struct util_list *list);

/**
 * Pop (get and remove) the head of the list
 *
 * @param list List to pop
 * @return Removed head element of the list (or null if empty)
 */
struct util_list_node *util_list_pop_head(struct util_list *list);

/**
 * Remove a specific element of the list. If the element is not part of the,
 * the list is not modified.
 *
 * @param list List to remove an element from
 * @param node Element to remove from the list
 */
void util_list_remove(struct util_list *list, struct util_list_node *node);

/**
 * Check if a list is empty
 *
 * @param list List to check
 * @return True if empty, false otherwise
 */
bool util_list_empty(struct util_list *list);

#endif
