/*
 * simgen.c
 *
 * Process arrival generator to be used with a simulator,
 * for A#3, CSC 360 Summer 2018
 *
 * Prepared by: Michael Zastre (University of Victoria) 
 * Summer 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define SHORT_TASK_MIN 5
#define SHORT_TASK_MAX 25
#define LONG_TASK_MIN  50
#define LONG_TASK_MAX  1000

/* 
 * The *lower* this number, the fewer short tasks will be
 * generated randomly.
 */
#define SHORT_LONG_THRESHOLD 0.0

#define ARRIVAL_TIME_RANGE_SMALL 10
#define ARRIVAL_TIME_RANGE_LARGE 500

/* The *lower* this number, the fewer small intervals between
 * task arrivals.
 */
#define LARGE_SMALL_THRESHOLD 0.2



float generate_task_length()
{
    float short_long_choice = rand();
    float length = rand();

    if ((short_long_choice / RAND_MAX) > SHORT_LONG_THRESHOLD) {
        return (length / RAND_MAX 
            * (LONG_TASK_MAX - LONG_TASK_MIN) + LONG_TASK_MIN);
    } else {
        return (length / RAND_MAX 
            * (SHORT_TASK_MAX - SHORT_TASK_MIN) + SHORT_TASK_MIN);
    }
}


int generate_arrival_interval()
{
    float large_small_choice = rand();
    large_small_choice /= RAND_MAX;

    float length = rand();

    if (large_small_choice < LARGE_SMALL_THRESHOLD) {
        return (int)(length / RAND_MAX
            * (ARRIVAL_TIME_RANGE_SMALL - 1) + 1);
    } else {
        return (int)(length / RAND_MAX 
            * (ARRIVAL_TIME_RANGE_LARGE - 1) + 1);
    }
}


int main(int argc, char *argv[]) {
    int i;

    if (argc < 3) {
        fprintf(stderr,
            "usage: %s <number of tasks> <random seed>\n", argv[0]);
        exit(1); 
    }
   
    int num_tasks = atoi(argv[1]);
    int random_seed = atoi(argv[2]);
    srand(random_seed);

    int   task_arrival_time = 0;
    float task_length = 0.0;
    for (i = 0; i < num_tasks; i++) {
        task_arrival_time += generate_arrival_interval();
        task_length = generate_task_length();
        printf("%03d %05d %.2f\n", i, task_arrival_time,
            task_length);
    }

    exit(0);
}
