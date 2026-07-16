#pragma once
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

typedef enum _task_status {
    task_status_todo,
    task_status_in_progress,
    task_status_done
} task_status;

typedef int32_t task_id;
typedef struct _task {
    task_id     id;
    char       *description;
    task_status status;
    time_t      created_at;
    time_t      updated_at;
} task;

void task_init(task *task, const char *description);
void task_regenerate_id(task *task);
void task_copy(task *dest, const task *src);
