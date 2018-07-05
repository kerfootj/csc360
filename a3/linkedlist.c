/*
 * linkedlist.c
 *
 * Based on the implementation approach described in "The Practice 
 * of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

/*
 * Wrapper around malloc() -- ends the program if malloc() fails
 * (i.e., eliminates the need of an "if" statement around each
 * call to malloc() in the main body of code).
 */
void *emalloc(size_t n) {
    void *p;

    p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "malloc of %zu bytes failed", n);
        exit(1);
    }

    return p;
}


/*
 * New linked-list node. Beware as the only field that gets a
 * specific value in this function is the "next" field; everything
 * else is uninitialized.
 */
taskval_t *new_task() {
    taskval_t *new_t;

    new_t = (taskval_t *) emalloc(sizeof(taskval_t));
    new_t->next = NULL;

    return new_t;
}


/*
 * De-allocate memory for a linked-list node.
 */
void end_task(taskval_t *t) {
    assert (t != NULL);
    free(t);
}


/*
 * At the end of this operation, the node passed in will be at the
 * head of the returned list. Note that "list" here really is an
 * address to some taskval_t.
 */
taskval_t *add_front(taskval_t *list, taskval_t *task) {
    task->next = list;
    return task;
}


/*
 * At the end of this operation, the node passed in will be at
 * the tail of the returned list. 
 */
taskval_t *add_end(taskval_t *list, taskval_t *task) {
    taskval_t *curr;

    if (list == NULL) {
        task->next = NULL;
        return task;
    }

    for (curr = list; curr->next != NULL; curr = curr->next);
    curr->next = task;
    task->next = NULL;
    return list;
}


/*
 * Return the address fof the taskval_t at the head of the list.
 * This does *not* modify the list.
 */
taskval_t *peek_front(taskval_t *list) {
    return list;
}


/*
 * Return the list that results from removing the head node from
 * the list.
 */
taskval_t *remove_front(taskval_t *list) {
    if (list == NULL) {
        return NULL;
    }

    return list->next;
}


/*
 * Given some list, apply the second parameter -- a function -- to
 * each node on the list. The third parameter is used for any additional
 * argument needed by the passed-in function. (The third value can, 
 * in practice, be NULL).
 */
void apply(taskval_t *list,
           void (*fn)(taskval_t *list, void *),
           void *arg)
{
    for ( ; list != NULL; list = list->next) {
        (*fn)(list, arg);
    }
}
