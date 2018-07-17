/****************************************************************************
 * File Name        : rrsim.c                                               *
 * Class            : CSC 360                                               *
 * Assignment       : 3                                                     *
 * Date             : July 17, 2018                                         *
 * Author           : Joel kerfootj                                         *
 * Git Repo         : https://github.com/kerfootj/csc360                    *
 ****************************************************************************/

/********************************* LIBRARIES ********************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

/********************************* CONSTANTS ********************************/
#define MAX_BUFFER_LEN 80

/****************************** GLOBAL VARIABLES ****************************/
taskval_t *event_list = NULL;
int time = 0;

/******************************* IMPLEMENTATION *****************************/
void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}


/*
 * Function: print_time
 * --------------------
 * prints the current time
 * must be called every tick as print_time is the only place time is incrimented
 */
void print_time() {
    printf("[%04d] ", time);
    time++;
}


/*
 * Function: print_task
 * --------------------
 * prints the ammount of cpu used vs requested for the given task
 *
 * *t       : the task that was completed
 */
void print_task(taskval_t *t, void *arg) {
    printf("id=%04d req=%3.2f used=%3.2f\n",
        t->id,
        t->cpu_request,
        t->cpu_used
    );  
}


/*
 * Function: print_task_comlete
 * ----------------------------
 * prints the waiting time and turn around time for the task
 *
 * *t       : the task that was completed
 */
void print_task_complete(taskval_t *t) {
    printf("id=%04d EXIT w=%.2f ta=%.2f\n", 
        t->id,
        (t->finish_time - t->arrival_time) - t->cpu_request,
        (t->finish_time - t->arrival_time) * 1.0
    );
}


/*
 * Function: dispatch_task
 * -----------------------
 * simulates the cost of dispatching a task
 *
 * dlen     : cost of dispatching tasks in ticks
 */
void dispatch_task(int dlen) {
    int i;
    for (i=0; i<dlen; i++) {
        print_time();
        printf("DISPATCHING\n");
    }
}


/*
 * Function:    run_task
 * ---------------------
 * runs the task until it's quantum expires or the task completes
 *
 * *t       : the task to run
 * qlen     : length of the quantum in ticks    
 *
 * returns  : 1 if task completed
 *          : 0 if task used up quantum but didn't complete task
 */
int run_task(taskval_t *t, int qlen) {
    float rem_time = t->cpu_request - t->cpu_used;
    float run_time = (rem_time > qlen) ? qlen : rem_time;
    run_time = (run_time < 1) ? 1 : run_time;

    int complete = 0;

    int i;
    for (i=0; i<run_time; i++) {
        t->cpu_used = t->cpu_used +1;
        print_time();
       
        if (t->cpu_used >= t->cpu_request) {

            t->cpu_used = t->cpu_request;
            t->finish_time = time -1;
            print_task_complete(t);
            complete = 1;

        } else {
            print_task(t, NULL);
        }
    }
    return complete;
}


/*
 * Function:    run_simulation
 * ---------------------------
 * simulates round robin cpu scheduling on the given tasks
 * exits once all tasks are completed
 *
 * qlen     : length of the quantum in ticks    
 * dlen     : cost of dispatching tasks in ticks
 */
void run_simulation(int qlen, int dlen) {
    taskval_t *ready_q = NULL;
    taskval_t *incoming = NULL;
    taskval_t *current = NULL;
    taskval_t *temp = NULL;
    
    int status = 1;

    for (;;) {
        
        current = peek_front(ready_q);

        // All tasks arrived and completed
        if (incoming == NULL && current == NULL && status == 0) {
           break;
        }

        for (;;) {
            incoming = peek_front(event_list);
            if (incoming != NULL) {
                // Task arrived
                if (incoming->arrival_time <= time) {
                    // Create new task and add to ready queue
                    temp = new_task();

                    temp->id = incoming->id;
                    temp->arrival_time = incoming->arrival_time;
                    temp->finish_time = 0;
                    temp->cpu_request = incoming->cpu_request;
                    temp->cpu_used = 0;
                    temp->next = incoming->next;
                    
                    ready_q = add_end(ready_q, temp);
                    event_list = remove_front(event_list);
                } else {
                    break;
                }
            } else {
                break;
            }
        }   
        
        current = peek_front(ready_q);

        // Dispatch task if avalibe
        if (current != NULL) {
            dispatch_task(dlen);
            // Task completed, end the task
            if (run_task(current, qlen)) {
                ready_q = remove_front(ready_q);
                end_task(current);
                if (ready_q != NULL)
                    time = time -1;
                status = 0;
            // Task used quantum, return in to the back of the queue
            } else {
                ready_q = remove_front(ready_q);
                ready_q = add_end(ready_q, current);
                status = 1;
            }
        // CPU was idle
        } else {
            print_time();
            printf("IDLE\n");
        }
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
