#include "clipboard-manager_list.h"
#include "clipboard-manager_error.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

typedef struct Node *node_t;

struct Node
{
    char *data;
    node_t prev, next;
};

struct cm_List
{
    node_t head, tail;
};

static cm_list_t slist = NULL;
static node_t siter = NULL;

static node_t node_init(const char *text)
{
    node_t node = malloc(sizeof *node);
    if (!node)
    {
        cm_error_handle(ENOMEM);
        return NULL;
    }
    size_t len_text = strlen(text) + 1;
    node->data = malloc(len_text);
    if (!node->data)
    {
        cm_error_handle(ENOMEM);
        free(node);
        return NULL;
    }
    if(strcpy_s(node->data, len_text, text)){
        free(node->data);
        free(node);
        cm_error_handle(errno);
        return NULL;
    }
    node->prev = node->next = NULL;
    return node;
}
static void node_destroy(node_t node)
{
    if (node->data)
        free(node->data);
    free(node);
}
cm_list_t cm_list_init()
{
    cm_list_t list = malloc(sizeof *list);
    if (!list)
    {
        cm_error_handle(ENOMEM);
        return NULL;
    }
    list->head = list->tail = NULL;
    return list;
}
bool cm_list_push(cm_list_t list, const char *text)
{
    if (!list || !text)
    {
        cm_error_handle(EINVAL);
        return false;
    }
    if (!list->head)
    {
        list->head = list->tail = node_init(text);
        if (!list->head)
            return false;
        return true;
    }
    list->tail->next = node_init(text);
    if (!list->tail->next)
        return false;
    list->tail->next->prev = list->tail;
    list->tail = list->tail->next;
    return true;
}
bool cm_list_push_front(cm_list_t list, const char *text)
{
    if (!list || !text)
    {
        cm_error_handle(EINVAL);
        return false;
    }
    if (!list->head)
    {
        list->head = list->tail = node_init(text);
        if (!list->head)
            return false;
        return true;
    }
    list->head->prev = node_init(text);
    if(!list->head->prev)
        return false;
    list->head->prev->next = list->head;
    list->head = list->head->prev;
    return true;
}
bool cm_list_clear(cm_list_t list){
    if(!list){
        cm_error_handle(EINVAL);
        return false;
    }
    node_t iter = list->head, temp = NULL;
    while (iter){
        temp = iter;
        iter = iter->next;
        node_destroy(temp);
    }
    list->head = list->tail = NULL;
    return true;
}
bool cm_list_destroy(cm_list_t *listptr){
    if(!listptr){
        cm_error_handle(EINVAL);
        return false;
    }
    if(!cm_list_clear(*listptr))
        return false;
    free(*listptr);
    *listptr = NULL;
    return true;
}
bool cm_list_write(cm_list_t list, void *file){
    if(!list || !file){
        cm_error_handle(EINVAL);
        return false;
    }
    node_t iter = list->head;
    while (iter){
        fprintf_s(file, "%s\n", iter->data);
        iter = iter->next;
    }
    return true;
}
node_t iter = NULL;
char *cm_list_each(cm_list_t list){
    if(list){
        iter = list->head;
        if(!iter) return NULL;
        char *data = iter->data;
        iter = iter->next;
        return data;
    }
    if(!iter) return NULL;
    char *data = iter->data;
    iter = iter->next;
    return data;
}