#include <assert.h>
#include <corecrt_search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "cjson/cJSON.h"
#include "debug.h"

#define _CHECK_JSON(expr) do { if (!(expr)) { return false; } } while(false)

void task_init(task *task, const char* description) {
    EXPECTS(task != NULL, "Expected valid task pointer");
    EXPECTS(description != NULL, "Expected valid description string");
    if(!(task && description)) {
        return;
    }

    time(&task->created_at);

    const size_t desc_buffer_len = (strlen(description) * sizeof(char)) + 1;
    task->description = calloc(desc_buffer_len, sizeof(char));
    memset(task->description, 0, desc_buffer_len);
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

bool task_serialize(const task *task, cJSON *obj) {
    EXPECTS(task != NULL, "Expected valid task");
    EXPECTS(obj != NULL, "Expected valid JSON object");

    if(!(task && obj)) {
        return false;
    }

    _CHECK_JSON(cJSON_AddStringToObject(obj, "description", task->description));
    _CHECK_JSON(cJSON_AddNumberToObject(obj, "status", task->status));
    _CHECK_JSON(cJSON_AddNumberToObject(obj, "created_at", task->created_at));
    _CHECK_JSON(cJSON_AddNumberToObject(obj, "updated_at", task->updated_at));
    return true;
}

bool task_deserialize(task *task, const cJSON *obj) {
    EXPECTS(task != NULL, "Expected valid task");
    EXPECTS(obj != NULL, "Expected valid JSON object");

    if(!(task && obj)) {
        return false;
    }

    cJSON *description = cJSON_GetObjectItem(obj, "description");
    cJSON *status = cJSON_GetObjectItem(obj, "status");
    cJSON *created_at = cJSON_GetObjectItem(obj, "created_at");
    cJSON *updated_at = cJSON_GetObjectItem(obj, "updated_at");

    EXPECTS(description != NULL, "Expected JSON object to have 'description' key");
    EXPECTS(status != NULL, "Expected JSON object to have 'status' key");
    EXPECTS(created_at != NULL, "Expected JSON object to have 'created_at' key");
    EXPECTS(updated_at != NULL, "Expected JSON object to have 'updated_at' key");

    if(!(description && status && created_at && updated_at)) {
        return false;
    }

    EXPECTS(cJSON_IsString(description), "Expected description JSON to be string");
    if(!cJSON_IsString(description)) {
        return false;
    }
    EXPECTS(cJSON_IsNumber(status), "Expected status JSON to be number");
    if(!cJSON_IsNumber(status)) {
        return false;
    }
    EXPECTS(cJSON_IsNumber(created_at), "Expected created_at JSON to be number");
    if(!cJSON_IsNumber(created_at)) {
        return false;
    }
    EXPECTS(cJSON_IsNumber(updated_at), "Expected updated_at JSON to be number");
    if(!cJSON_IsNumber(updated_at)) {
        return false;
    }

    const size_t desc_len = strlen(description->valuestring);
    task->description = malloc(desc_len + 1);
    memset(task->description, 0, desc_len + 1);

    strcpy_s(task->description, desc_len + 1, description->valuestring);

    task->status = status->valueint;
    task->created_at = created_at->valueint;
    task->updated_at = updated_at->valueint;
    return true;
}
