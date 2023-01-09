#ifndef ORANGE_OS_INCLUDE_LIST_H
#define ORANGE_OS_INCLUDE_LIST_H
#include "stddef.h"

typedef struct list_node {
  struct list_node *prev;
  struct list_node *next;
} list_node_t;

static inline void list_init(list_node_t *head) {
  head->prev = head;
  head->next = head;
}

/* list_rpush() - insert node after head */
static inline void list_rpush(list_node_t *head, list_node_t *node) {
  node->prev = head;
  node->next = head->next;
  head->next->prev = node;
  head->next = node;
}

/* list_lpush() - insert node before head */
static inline void list_lpush(list_node_t *head, list_node_t *node) {
  node->next = head;
  node->prev = head->prev;
  head->prev->next = node;
  head->prev = node;
}

/* list_rpop() - pop the node after head */
static inline list_node_t *list_rpop(list_node_t *head) {
  list_node_t *node = head->next;
  node->next->prev = head;
  head->next = node->next;
  return node;
}

static inline list_node_t *list_lpop(list_node_t *head) {
  list_node_t *node = head->prev;
  node->prev->next = head;
  head->prev = node->prev;
  return node;
}

static inline int list_empty(list_node_t *head) {
  return head->next == head;
}

#endif //ORANGE_OS_INCLUDE_LIST_H
