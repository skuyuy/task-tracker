#pragma once
#include <stdbool.h>
#include <stddef.h>

#include "task.h"

typedef struct _tl_node {
    task value;
    struct _tl_node *next;
} tl_node;

typedef struct _task_list {
    tl_node **slots;
    size_t    slot_count;
} task_list;

void  task_list_init(task_list *list, const size_t slots);
void  task_list_clear(task_list *list);
task* task_list_find(const task_list *list, const char *desc);
bool  task_list_append(task_list *list, const char *desc);
bool  task_list_insert(task_list *list, const task *task);
bool  task_list_remove(task_list *list, const char *desc);
