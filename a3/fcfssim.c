#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

#define MAX_BUFFER_LEN 80

taskval_t *event_list = NULL;
int time = 0;

void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}


void print_time() {
    printf("[%04d] ", time);
    time++;
}


void print_task(taskval_t *t, void *arg) {
    printf("id=%04d req=%3.2f used=%3.2f\n",
        t->id,
        t->cpu_request,
        t->cpu_used
    );  
}


void print_task_complete(taskval_t *t, void *arg) {
    printf("id=%04d EXIT w=%.2f ta=%.2f\n", 
        t->id,
        (t->finish_time - t->arrival_time) - t->cpu_request,
        (t->finish_time - t->arrival_time) * 1.0
    );
}


void dispatch_task(int dlen) {
    int i;
    for (i=0; i<dlen; i++) {
        print_time();
        printf("DISPATCHING\n");
    }
}

void run_task(taskval_t *t) {
	float run_time = t->cpu_request;
	int i;

	for (i=0; i<run_time; i++) {
		t->cpu_used = t->cpu_used +1;
		print_time();

		if (t->cpu_used >= t->cpu_request) {

            t->cpu_used = t->cpu_request;
            t->finish_time = time -1;
            print_task_complete(t, NULL);

        } else {
            print_task(t, NULL);
        }
	}
}


void run_simulation(int qlen, int dlen) {
    taskval_t *current = NULL;
    int status = 0;

    for (;;) {
    	
    	current = peek_front(event_list);
    	
    	if (current == NULL && status == 0) {
    		break;
    	}    	

    	if (current->arrival_time <= time) {
    		dispatch_task(dlen);
    		run_task(current);
    		event_list = remove_front(event_list);
    		end_task(current);
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