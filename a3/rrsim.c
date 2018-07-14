#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

#define MAX_BUFFER_LEN 80

taskval_t *event_list = NULL;

void print_time(int time) {
    printf("[%04d] ", time);
}


void print_task(taskval_t *t, void *arg) {
    printf("task %03d: %5d %3.2f %3.2f\n",
        t->id,
        t->arrival_time,
        t->cpu_request,
        t->cpu_used
    );  
}


void dispatch_task(int *time, int dlen) {
    for (int i=0; i<dlen; i++) {
        print_time(*time++);
        printf("DISPATCHING\n");
    }
}


int run_task(taskval_t *t, int *time, int qlen) {
    int rem_time = t->cpu_request - t->cpu_used;
    int run_time = (rem_time > qlen) ? qlen : rem_time;

    for (int i=0; i<run_time; i++) {
        print_time(time++);
        print_task(t, NULL);
    }
}


void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}


void run_simulation(int qlen, int dlen) {
    taskval_t *ready_q = NULL;
    taskval_t *incoming = NULL;
    taskval_t *current = NULL;
    
    int status = 0;
    int time = 0;

    while (1) {
        incoming = peek_front(event_list);
        if (incoming != NULL) {
            // Task arrived
            if (incoming->arrival_time == time) {
                ready_q = add_end(ready_q, incoming);
                event_list = remove_front(event_list);
            }
        }

        current = peek_front(ready_q);
        if (current != NULL) {
            dispatch_task(&time, dlen);

        }
        time++;
    }
}


int main(int argc, char *argv[]) {
    char   input_line[MAX_BUFFER_LEN];
    int    i;
    int    task_num;
    int    task_arrival;
    float  task_cpu;
    int    quantum_length = -1;
    int    dispatch_length = -1;

    taskval_t *temp_task;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quantum") == 0 && i+1 < argc) {
            quantum_length = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "--dispatch") == 0 && i+1 < argc) {
            dispatch_length = atoi(argv[i+1]);
        }
    }

    if (quantum_length == -1 || dispatch_length == -1) {
        fprintf(stderr, 
            "usage: %s --quantum <num> --dispatch <num>\n",
            argv[0]);
        exit(1);
    }


    while(fgets(input_line, MAX_BUFFER_LEN, stdin)) {
        sscanf(input_line, "%d %d %f", &task_num, &task_arrival, &task_cpu);
        temp_task = new_task();
        temp_task->id = task_num;
        temp_task->arrival_time = task_arrival;
        temp_task->cpu_request = task_cpu;
        temp_task->cpu_used = 0.0;
        event_list = add_end(event_list, temp_task);
    }

#ifdef DEBUG
    int num_events;
    apply(event_list, increment_count, &num_events);
    printf("DEBUG: # of events read into list -- %d\n", num_events);
    printf("DEBUG: value of quantum length -- %d\n", quantum_length);
    printf("DEBUG: value of dispatch length -- %d\n", dispatch_length);
#endif

    run_simulation(quantum_length, dispatch_length);

    return (0);
}
