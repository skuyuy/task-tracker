#pragma once
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#include "cjson/cJSON.h"

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

bool task_serialize(const task *task, cJSON *obj);
bool task_deserialize(task *task, const cJSON *obj);
