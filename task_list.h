#pragma once
#include <stdbool.h>

#include "task.h"

typedef struct _tl_node {
    task value;
    struct _tl_node *next;
} tl_node;

typedef struct _task_list {
    tl_node *head;
    tl_node *tail;
    size_t   len;
} task_list;

void  task_list_init(task_list *list);
void  task_list_clear(task_list *list);
task* task_list_find(const task_list *list, const task_id id);
bool  task_list_append(task_list *list, task *task);
bool  task_list_remove(task_list *list, const task_id id);
