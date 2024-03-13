#ifndef CLIPBOARD_MANAGER_LIST_H
#define CLIPBOARD_MANAGER_LIST_H

#include <stdbool.h>

typedef struct cm_List *cm_list_t;

extern cm_list_t cm_list_init();
extern bool cm_list_push(cm_list_t, const char *);
extern bool cm_list_push_front(cm_list_t, const char *);
extern bool cm_list_clear(cm_list_t);
extern bool cm_list_destroy(cm_list_t *);
extern bool cm_list_write(cm_list_t, void *file);
extern char *cm_list_each(cm_list_t list);

#endif