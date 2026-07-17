#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "task_list.h"
#include "debug.h"
#include "task.h"
#include "hash.h"

tl_node *_tl_node_create(const char *desc) {
    if(!desc) {
        return NULL;
    }

    tl_node *node = malloc(sizeof(tl_node));
    if(!node) {
        return node;
    }

    task_init(&node->value, desc);
    node->next = NULL;

    return node;
}

void _tl_node_destroy(tl_node *node) {
    if(!node) {
        return;
    }

    tl_node *current = node->next;
    tl_node *prev = NULL;
    while(current) {
        prev = current;
        current = current->next;
        free(prev);
    }
    free(node);
}

size_t _tl_key(const task_list *list, const char *key) {
    size_t hash = hash_fnv1a_32(key, strlen(key));
    ENSURES(hash % list->slot_count < list->slot_count, "Key can not be greater than slot count");
    return hash % list->slot_count;
}

tl_node * _tl_slot_get(const task_list *list, const size_t slot) {
    if(!list || !list->slots || list->slot_count == 0 || slot >= list->slot_count) {
        return NULL;
    }
    return list->slots[slot];
}

void task_list_init(task_list *list, const size_t slots) {
    EXPECTS(list != NULL, "Expected valid list pointer");

    if(!list) {
        return;
    }

    list->slots = calloc(slots, sizeof(tl_node*));
    list->slot_count = slots;
    ENSURES(list->slots != NULL, "Could not allocate %s slots", slots);
}

void task_list_clear(task_list *list) {
    EXPECTS(list != NULL, "Expected valid list pointer");
    if(!list) {
        return;
    }

    for(size_t i = 0; i < list->slot_count; ++i) {
        _tl_node_destroy(list->slots[i]);
    }

    list->slots = NULL;
    list->slot_count = 0;
}

task *task_list_find(const task_list *list, const char *desc) {
    EXPECTS(list != NULL, "Expected valid list pointer");
    if(!list || list->slot_count == 0) {
        return NULL;
    }

    const size_t key = _tl_key(list, desc);
    tl_node *node = _tl_slot_get(list, key);
    if(!node) {
        return NULL;
    }

    // slot found - if there is no "next", we can immediately return
    if(!node->next) {
        return &node->value;
    }

    // slot found but there are more than one nodes; search through the bucket for the key
    // since we dont expect to have many collisions, linear search will suffice
    tl_node *current = node;
    while(current) {
        if(strcmp(current->value.description, desc) == 0) {
            // found exact key
            return &current->value;
        }
        current = current->next;
    }

    return NULL;
}

bool task_list_append(task_list *list, const char *desc) {
    EXPECTS(list != NULL, "Expected valid list pointer");
    EXPECTS(desc != NULL, "Expected valid key");

    if(!(list && desc)) {
        return false;
    }

    tl_node *node = _tl_node_create(desc);
    ENSURES(node != NULL, "Could not create node");
    if(!node) {
        return false;
    }

    size_t key = _tl_key(list, desc);
    tl_node *slot = _tl_slot_get(list, key);
    if(!slot) {
        list->slots[key] = node;
        return true;
    }

    tl_node *curr = slot;
    while(curr->next) {
        curr = curr->next;
    }
    curr->next = slot;

    return true;
}

bool task_list_remove(task_list *list, const char *desc) {
    EXPECTS(list != NULL, "Expected valid list pointer");
    EXPECTS(desc != NULL, "Expected valid key");

    if(!(list && list->slots && desc) || list->slot_count == 0 ) {
        return false;
    }

    const size_t key = _tl_key(list, desc);
    tl_node *node = _tl_slot_get(list, key);
    if(!node) {
        // node does not exist
        return false;
    }

    if(!node->next) {
        // slot found - if there is no "next", we can immediately remove it and reset the slot to NULL since it is empty now
        _tl_node_destroy(node);
        list->slots[key] = NULL;
        return true;
    }

    // slot found but there are more than one nodes; search through the bucket for the key
    tl_node *current = node;
    tl_node *prev = NULL;
    while(current) {
        prev = current;
        if(strcmp(current->value.description, desc) == 0) {
            // we found the node; destroy it and move the tail back
            _tl_node_destroy(current);
            prev->next = NULL;
            return true;
        }
        current = current->next;
    }

    return false;
}
