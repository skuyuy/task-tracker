#include <stdlib.h>

#include "task_list.h"
#include "debug.h"
#include "task.h"

tl_node *_tl_node_create(const task *task_ptr) {
    EXPECTS(task_ptr != NULL, "Expected valid task pointer");
    if(!task_ptr) {
        return NULL;
    }

    tl_node *node = malloc(sizeof(tl_node));
    ENSURES(node != NULL, "Failed to allocate task list node");
    if(!node) {
        return node;
    }

    task_copy(&node->value, task_ptr);
    node->next = NULL;
    return node;
}

void _tl_node_destroy(tl_node *node) {
    EXPECTS(node != NULL, "Expected valid node pointer");
    if(!node) {
        return;
    }
    free(node);
}

void task_list_init(task_list *list) {
    EXPECTS(list != NULL, "Expected valid list pointer");
    if(!list) {
        return;
    }

    list->len = 0;
    list->head = NULL;
}

void task_list_clear(task_list *list) {
    EXPECTS(list != NULL, "Expected valid list pointer");
    if(!list) {
        return;
    }

    tl_node *current = list->head;
    while(current != NULL) {
        tl_node *next = current->next;
        _tl_node_destroy(current);
        current = next;
    }
    list->len = 0;
}

task *task_list_find(const task_list *list, const task_id id) {
    EXPECTS(list != NULL, "Expected valid list pointer");
    if(!list || list->len == 0) {
        return NULL;
    }

    tl_node *current = list->head;
    while(current != NULL) {
        if(current->value.id == id) {
            return &current->value;
        }
        current = current->next;
    }
    return NULL;
}

bool task_list_append(task_list *list, task *task) {
    EXPECTS(list != NULL, "Expected valid list pointer");
    EXPECTS(task != NULL, "Expected valid task pointer");

    if(!(task && list)) {
        return false;
    }

    // check if any collision would occur; regenerate ID until we can not find the ID anymore
    while(task_list_find(list, task->id) != NULL) {
        task_regenerate_id(task);
    }

    tl_node *node = _tl_node_create(task);
    ENSURES(node != NULL, "Could not create node");
    if(!node) {
        return false;
    }

    if(list->len == 0) {
        list->head = node;
        list->tail = node;
        list->len = 1;
        return true;
    }

    EXPECTS(list->tail != NULL, "Expected valid list tail (illegal state)");
    if(list->tail == NULL) {
        _tl_node_destroy(node);
        return false;
    }

    list->tail->next = node;
    list->tail = node;
    list->len++;
    return true;
}

bool task_list_remove(task_list *list, const task_id id) {
    EXPECTS(list != NULL, "Expected valid list pointer");
    if(!list || list->len == 0) {
        return false;
    }

    if(list->len == 1 && list->head->value.id == id) {
        _tl_node_destroy(list->head);
        list->head = NULL;
        list->tail = NULL;
        list->len = 0;
        return true;
    }

    tl_node *current = list->head;
    tl_node *prev = NULL;

    while(current != NULL) {
        prev = current;
        tl_node *next = current->next;
        if(current->value.id == id) {
            prev->next = next;
            _tl_node_destroy(current);
            list->len -= 1;
            return true;
        }
        current = next;
    }
    return false;
}
