#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "debug.h"

void task_init(task *task, const char* description) {
    EXPECTS(task != NULL, "Expected valid task pointer");
    EXPECTS(description != NULL, "Expected valid description string");
    if(!(task && description)) {
        return;
    }

    time(&task->created_at);

    const size_t desc_buffer_len = (strlen(description) * sizeof(char)) + 1;
    task->description = calloc(desc_buffer_len, sizeof(char));
    strcpy_s(task->description, desc_buffer_len, description);

    task->status = task_status_todo;
    task->updated_at = task->created_at;

    ENSURES(task->created_at != (time_t)(-1), "Could not set created_at time");
    ENSURES(strcmp(task->description, description) == 0, "Could not copy description text \"%s\"", description);
}

void task_copy(task *dest, const task *src) {
    EXPECTS(dest != NULL, "Expected valid destination pointer");
    EXPECTS(src != NULL, "Expected valid source pointer");

    if(!(dest && src)) {
        return;
    }

    const size_t desc_buffer_len = strlen(src->description) * sizeof(char);
    if(dest->description) {
        realloc(dest->description, desc_buffer_len);
    } else {
        dest->description = malloc(desc_buffer_len);
    }

    strcpy_s(dest->description, desc_buffer_len, src->description);

    dest->status = src->status;
    dest->created_at = src->created_at;
    dest->updated_at = src->updated_at;

    ENSURES(strcmp(dest->description, src->description) == 0, "Could not copy source description to destination");
}
