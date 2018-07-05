#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

void print_task(taskval_t *t, void *arg) {
    printf("task %03d: %5d %3.2f %3.2f\n",
        t->id,
        t->arrival_time,
        t->cpu_request,
        t->cpu_used
    );
}


void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}


void test01() {
    char *title = "test01";

    taskval_t *list = NULL;
    taskval_t *temp;
    int expected[] = {99, 5, 3, 42};
    int size = 4;
    int i = 0;
    int len;

    temp = new_task();
    temp->id = 5;
    list = add_front(list, temp);

    temp = new_task();
    temp->id = 3;
    list = add_end(list, temp);

    temp = new_task();
    temp->id = 99;
    list = add_front(list, temp);

    temp = new_task();
    temp->id = expected[i++];
    temp->id = 42;
    list = add_end(list, temp);

    apply(list, print_task, NULL);
    len = 0;
    apply(list, increment_count, &len);
    printf("%s -- length: %d\n", title,len);

    for (i = 0; i < size; i++) {
        temp = peek_front(list);
        if (temp->id != expected[i]) {
            fprintf(stderr, "%s -- %d: expected %d, got instead %d\n",
                title, i, expected[i], temp->id);
            fprintf(stdout, "%s: FAILED\n", title);
            return;
        }
        list = remove_front(list);
        end_task(temp);
    }

    fprintf(stdout, "%s: passed\n", title);

    return;
}


void test02() {
    char *title = "test02";

    taskval_t *list = NULL;
    taskval_t *temp;
    int size = 100;
    int i = 0;

    for (i = 0; i < size; i++) {
        temp = new_task();
        temp->id = i;
        list = add_end(list, temp);
    }

    for (i = 0; i < size; i++) {
        temp = peek_front(list);
        if (temp->id != i) {
            fprintf(stderr, "%s -- %d: expected %d, got instead %d\n",
                title, i, i, temp->id);
            fprintf(stdout, "%s: FAILED\n", title);
            return;
        }
        list = remove_front(list);
        end_task(temp);
    }

    fprintf(stdout, "%s: passed\n", title);

    return;
}


void test03() {
    char *title = "test03";

    taskval_t *list = NULL;
    taskval_t *temp;
    int size = 100;
    int i = 0;

    for (i = 0; i < size; i++) {
        temp = new_task();
        temp->id = i;
        list = add_front(list, temp);
    }

    for (i = size-1; i >= 0; i--) {
        temp = peek_front(list);
        if (temp->id != i) {
            fprintf(stderr, "%s -- %d: expected %d, got instead %d\n",
                title, i, i, temp->id);
            fprintf(stdout, "%s: FAILED\n", title);
            return;
        }
        list = remove_front(list);
        end_task(temp);
    }

    fprintf(stdout, "%s: passed\n", title);

    return;
}


int main(int argc, char *argv[]) {
    test01();
    test02();
    test03();

    return (0);
}
