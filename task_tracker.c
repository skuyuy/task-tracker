#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task_list.h"
#include "debug.h"

bool _load_tasks(task_list *list) {
    EXPECTS(list != NULL, "Expected valid list");
    if(!list) {
        return false;
    }
    return true;
}

bool _save_tasks(task_list *list) {
    EXPECTS(list != NULL, "Expected valid list");
    if(!list) {
        return false;
    }
    return true;
}

void task_cmd_help() {
    printf(
        "usage: task-tracker <command> [<args>...]\n"
        "  commands:\n"
        "    add description       - Adds a new task with the given description\n"
        "    update id description - Updates the description of the task with the given ID\n"
        "    delete id             - Deletes the task with the given id\n"
        "    in-progress id        - Marks the task with the given id as \"In Progress\"\n"
        "    done id               - Marks the task with the given id as \"Done\"\n"
        "    list [filter]         - Lists all tasks matching a filter. If no filter is given, lists all tasks\n"
   );
}

int task_cmd_add(task_list *list, const char *description) {
    EXPECTS(description != NULL, "Expected valid description string");
    if(!description) {
        printf(
            "No description provided.\n"
            "usage: task-tracker add ..."
        );
        return EXIT_SUCCESS;
    }

    task_list tasks;
    task_list_init(&tasks);

    if(!_load_tasks(&tasks)) {
        fprintf(stderr, "Could not load tasks");
        return EXIT_FAILURE;
    }

    task task;
    task_init(&task, description);
    if(!task_list_append(&tasks, &task)) {
        fprintf(stderr, "Could not add task");
        return EXIT_FAILURE;
    }

    if(!_save_tasks(&tasks)) {
        fprintf(stderr, "Could not save tasks");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int task_cmd_update(task_list *list, const task_id id, const char *description) {
    fprintf(stderr, "Not implemented");
    return EXIT_FAILURE;
}

int task_cmd_delete(task_list *list, const task_id id) {
    fprintf(stderr, "Not implemented");
    return EXIT_FAILURE;
}

int task_cmd_mark_in_progress(task_list *list, const task_id id) {
    fprintf(stderr, "Not implemented");
    return EXIT_FAILURE;
}

int task_cmd_mark_done(task_list *list, const task_id id) {
    fprintf(stderr, "Not implemented");
    return EXIT_FAILURE;
}

int task_cmd_list(task_list *list, const char* filter) {
    fprintf(stderr, "Not implemented");
    return EXIT_FAILURE;
}

int _hint_improper_usage(const char *reason) {
    printf("Improper usage of task-tracker: %s\n", reason);
    task_cmd_help();
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    if(argc <= 1) {
        return _hint_improper_usage("Not enough arguments");
    }

    task_list tasks;
    task_list_init(&tasks);

    if(!_load_tasks(&tasks)) {
        fprintf(stderr, "Could not load tasks");
        return EXIT_FAILURE;
    }

    const char *command = argv[1];
    int result;

    if(strcmp(command, "help") == 0) {
        task_cmd_help();
        return EXIT_SUCCESS;
    } else if (strcmp(command, "add") == 0) {
        if(argc != 3) {
            return _hint_improper_usage("Missing task description");
        }

        result = task_cmd_add(&tasks, argv[2]);
    } else if (strcmp(command, "update") == 0) {
        if(argc != 4) {
            return _hint_improper_usage("Missing task ID");
        }

        result = task_cmd_update(&tasks, atoi(argv[2]), argv[3]);
    } else if (strcmp(command, "delete") == 0) {
        if(argc != 3) {
            return _hint_improper_usage("Missing task ID");
        }

        result = task_cmd_delete(&tasks, atoi(argv[2]));
    } else if (strcmp(command, "in-progress") == 0) {
        if(argc != 3) {
            return _hint_improper_usage("Missing task ID");
        }

        result = task_cmd_mark_in_progress(&tasks, atoi(argv[2]));
    } else if (strcmp(command, "done") == 0) {
        if(argc != 3) {
            return _hint_improper_usage("Missing task ID");
        }

        result = task_cmd_mark_done(&tasks, atoi(argv[2]));
    } else if (strcmp(command, "list") == 0) {
        if (argc == 2) {
            // list all
            return task_cmd_list(&tasks, NULL);
        } else if (argc == 3) {
            // list with filter
            return task_cmd_list(&tasks, argv[2]);
        } else {
            return _hint_improper_usage("Too many arguments");
        }
    } else {
        return _hint_improper_usage("Command not found");
    }

    if(!_save_tasks(&tasks)) {
        fprintf(stderr, "Could not save tasks");
        return EXIT_FAILURE;
    }
}

#include "debug.c"
#include "task.c"
#include "task_list.c"
