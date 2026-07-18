#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cjson/cJSON.h"
#include "task.h"
#include "task_list.h"
#include "debug.h"

// #define _TASKS_TEST_DATA

/*
 * DATA FILE model
 *
 * task -> saved to <...>.task
 *
 * filename = hash
 * content:
 *
 * description len (long, 8 bytes)
 * description characters (char*, #len bytes)
 * status (int, 4 bytes)
 * created_at (int64, 8 bytes)
 * updated_at (int64, 8 bytes)
 *
 * [][][][][][][][][]......[][][][][][][][][][][][][][][][][][][][][]
 * *------len-----**--desc--**status**--created_at--**--updated_at--*
 */

bool _load_tasks(task_list *list) {
    EXPECTS(list != NULL, "Expected valid list");
    if(!list) {
        return false;
    }

    FILE* file = NULL;
    if(fopen_s(&file, "task_tracker.json", "r") != 0) {
        return true; // there are just no files present
    }

    fseek(file, 0, SEEK_END);
    const long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(filesize);
    if(!buffer) {
        fclose(file);
        return false;
    }

    fread_s(buffer, filesize, 1, filesize, file);
    fclose(file);

    cJSON *json = cJSON_Parse(buffer);
    free(buffer);
    buffer = NULL;

    if(!json) {
        return false;
    }

    if(!cJSON_IsArray(json)) {
        cJSON_Delete(json);
        return false;
    }

    cJSON *elem = NULL;
    cJSON_ArrayForEach(elem, json) {
        task task = {};
        if(!task_deserialize(&task, elem)) {
            continue;
        }

        task_list_insert(list, &task);
    }
    cJSON_Delete(json);
    return true;
}

bool _save_tasks(task_list *list) {
    EXPECTS(list != NULL, "Expected valid list");
    if(!list) {
        return false;
    }

    FILE* file = NULL;
    if(fopen_s(&file, "task_tracker.json", "w") != 0) {
        return false;
    }

    cJSON *tasks_json = cJSON_CreateArray();
    if(!tasks_json) {
        fclose(file);
        return false;
    }

    for(int i = 0; i < list->slot_count; ++i) {
        tl_node *slot = list->slots[i];

        if(!slot) {
            continue;
        }

        while(slot) {
            cJSON *task_json = cJSON_CreateObject();
            if(!task_serialize(&slot->value, task_json)) {
                continue;
            }

            cJSON_AddItemToArray(tasks_json, task_json);
            slot = slot->next;
        }
    }

    char* content = cJSON_Print(tasks_json);
    bool result = false;
    if(content && fprintf_s(file, "%s", content) != 0) {
        result = true;
    }

    cJSON_Delete(tasks_json);
    fclose(file);
    free(content);
    return result;
}

void task_cmd_help() {
    printf(
        "usage: task-tracker <command> [<args>...]\n"
        "  commands:\n"
        "    add <task>                  - Adds a new task with the given description\n"
        "    update <task> <description> - Updates the description of the task with the given ID\n"
        "    delete <task>               - Deletes the task with the given id\n"
        "    in-progress <task>          - Marks the task with the given id as \"In Progress\"\n"
        "    done <task>                 - Marks the task with the given id as \"Done\"\n"
        "    list [<filter>]             - Lists all tasks matching a filter. If no filter is given, lists all tasks\n"
   );
}

int task_cmd_add(task_list *list, const char *description) {
    EXPECTS(description != NULL, "Expected valid description string");
    if(!description) {
        printf(
            "No description provided.\n"
            "usage: task-tracker add <description>\n"
        );
        return EXIT_FAILURE;
    }

    if(!task_list_append(list, description)) {
        fprintf(stderr, "Could not add task");
        return EXIT_FAILURE;
    }

    ENSURES(task_list_find(list, description) != NULL, "Key \"%s\" not found in task list", description);
    return EXIT_SUCCESS;
}

int task_cmd_update(task_list *list, const char *old_description, const char *new_description) {
    EXPECTS(old_description != NULL, "Expected valid old_description string");
    EXPECTS(new_description != NULL, "Expected valid new_description string");

    if(!old_description) {
        printf(
            "No task to replace provided.\n"
            "usage: task-tracker update <old_description> <new_description>\n"
        );
        return EXIT_FAILURE;
    }

    if(!new_description) {
        printf(
            "No replacement task provided.\n"
            "usage: task-tracker update <old_description> <new_description>\n"
        );
        return EXIT_FAILURE;
    }

    task *task = task_list_find(list, old_description);
    if(!task) {
        printf("Task \"%s\" not found.\n", old_description);
        return EXIT_FAILURE;
    }

    if(!task_list_remove(list, old_description)) {
        printf("Could not update task \"%s\"\n", old_description);
        return EXIT_FAILURE;
    }

    if(!task_list_append(list, new_description)) {
        printf("Could not update task \"%s\"\n", new_description);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int task_cmd_delete(task_list *list, const char *description) {
    EXPECTS(description != NULL, "Expected valid description string");
    if(!description) {
        printf(
            "No task to delete provided.\n"
            "usage: task-tracker delete <description>\n"
        );
        return EXIT_FAILURE;
    }

    if(!task_list_remove(list, description)) {
        printf("Could not delet task \"%s\"\n", description);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int _task_update(task_list *list, const char *description, const task_status status) {
    task *task = task_list_find(list, description);
    if(!task) {
        printf("Could not update task: Task \"%s\" does not exist\n", description);
        return EXIT_FAILURE;
    }

    if(task->status == status) {
        printf("Task \"%s\" already in progress\n", task->description);
    }

    task->status = status;
    time(&task->updated_at);
    return EXIT_SUCCESS;
}

int task_cmd_mark_in_progress(task_list *list, const char *description) {
    EXPECTS(description != NULL, "Expected valid description string");
    if(!description) {
        printf(
            "No task to update provided.\n"
            "usage: task-tracker in-progress <task>\n"
        );
        return EXIT_FAILURE;
    }

    return _task_update(list, description, task_status_in_progress);
}

int task_cmd_mark_done(task_list *list, const char *description) {
    EXPECTS(description != NULL, "Expected valid description string");
    if(!description) {
        printf(
            "No task to updateprovided.\n"
            "usage: task-tracker done <task>\n"
        );
        return EXIT_FAILURE;
    }

    return _task_update(list, description, task_status_done);
}

void _print_task(const task *task) {
    EXPECTS(task != NULL, "Expected valid task");
    if(!task) {
        return;
    }

    // copy dates so they are not directly modified
    struct tm created_at_utc;
    struct tm updated_at_utc;

    char created_at_str[12] = {};
    char updated_at_str[12] = {};

    gmtime_s(&created_at_utc, &task->created_at);
    gmtime_s(&updated_at_utc, &task->updated_at);
    strftime(created_at_str, sizeof(created_at_str) - 1, "%m/%d/%Y", &created_at_utc);
    strftime(updated_at_str, sizeof(updated_at_str) - 1, "%m/%d/%Y", &updated_at_utc);

    char *status_text = NULL;
    switch (task->status) {
        case task_status_todo:
            status_text = "Todo";
            break;
        case task_status_in_progress:
            status_text = "In progress";
            break;
        case task_status_done:
            status_text = "Done";
            break;
        default:
            status_text = "Unknown";
            break;
    }

    printf(
        "%s\n"
        "---------------------\n"
        "* created: %s\n"
        "* updated: %s\n"
        "* status:  %s\n"
        "\n",
        task->description,
        created_at_str,
        updated_at_str,
        status_text
    );
}

void task_cmd_list(task_list *list, const char* filter) {
    task_status filtered_status = -1;

    if(filter) {
        if(strcmp(filter, "todo") == 0) {
            filtered_status = task_status_todo;
        } else if (strcmp(filter, "in-progress") == 0) {
            filtered_status = task_status_in_progress;
        } else if (strcmp(filter, "done") == 0) {
            filtered_status = task_status_done;
        }
    }

    for(int i = 0; i < list->slot_count; ++i) {
        tl_node *slot = list->slots[i];
        if(!slot) {
            continue;
        }

        tl_node *curr = slot;
        while(curr) {
            // respect filter
            if(filtered_status > -1 && curr->value.status != filtered_status) {
                continue;
            }

            _print_task(&curr->value);
            curr = curr->next;
        }
    }
}

int _hint_improper_usage(const char *reason) {
    printf("Improper usage of task-tracker: %s\n", reason);
    task_cmd_help();
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    if(argc <= 1) {
        return _hint_improper_usage("Not enough arguments");
    }

    task_list tasks;
    task_list_init(&tasks, 1024);

#ifdef _TASKS_TEST_DATA
    // just push some tasks to test
    task_list_append(&tasks, "Test list command");
    task_list_append(&tasks, "Work on music");
    task_list_append(&tasks, "Learn the C programming language");
    task_list_append(&tasks, "Use more LLDB");
    task_list_append(&tasks, "Compile the code");
#endif

    if(!_load_tasks(&tasks)) {
        fprintf(stderr, "Could not load tasks");
        return EXIT_FAILURE;
    }

    const char *command = *(argv + 1);
    int result;

    if(strcmp(command, "help") == 0) {
        task_cmd_help();
        return EXIT_SUCCESS;
    } else if (strcmp(command, "add") == 0) {
        if(argc != 3) {
            return _hint_improper_usage("Missing task description");
        }

        result = task_cmd_add(&tasks, *(argv + 2));
    } else if (strcmp(command, "update") == 0) {
        if(argc != 4) {
            return _hint_improper_usage("Missing task ID");
        }

        result = task_cmd_update(&tasks, *(argv + 2), *(argv + 3));
    } else if (strcmp(command, "delete") == 0) {
        if(argc != 3) {
            return _hint_improper_usage("Missing task ID");
        }

        result = task_cmd_delete(&tasks, *(argv + 2));
    } else if (strcmp(command, "in-progress") == 0) {
        if(argc != 3) {
            return _hint_improper_usage("Missing task ID");
        }

        result = task_cmd_mark_in_progress(&tasks, *(argv + 2));
    } else if (strcmp(command, "done") == 0) {
        if(argc != 3) {
            return _hint_improper_usage("Missing task ID");
        }

        result = task_cmd_mark_done(&tasks, *(argv + 2));
    } else if (strcmp(command, "list") == 0) {
        if (argc == 2) {
            // list all
            task_cmd_list(&tasks, NULL);
            return EXIT_SUCCESS;
        } else if (argc == 3) {
            // list with filter
            task_cmd_list(&tasks, *(argv + 2));
            return EXIT_SUCCESS;
        } else {
            return _hint_improper_usage("Too many arguments");
        }
    } else {
        return _hint_improper_usage("Command not found");
    }

    if(result == EXIT_SUCCESS && !_save_tasks(&tasks)) {
        fprintf(stderr, "Could not save tasks");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#include "hash.c"
#include "debug.c"
#include "task.c"
#include "task_list.c"
#include "cjson/cJSON.c"
