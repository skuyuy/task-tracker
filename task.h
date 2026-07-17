#pragma once
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

typedef enum _task_status {
    task_status_todo,
    task_status_in_progress,
    task_status_done
} task_status;

typedef struct _task {
    char       *description;
    task_status status;
    time_t      created_at;
    time_t      updated_at;
} task;

void task_init(task *task, const char *description);
