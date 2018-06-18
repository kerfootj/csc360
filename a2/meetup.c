/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 */
int num;
int meet;
int count;
resource_t r;
sem_t t1;
sem_t t2;
sem_t mutex;

void initialize_meetup(int n, int mf) {
    char label[100];
    int i;

    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */

    num = n;
    meet = mf;
    count = 0;

    init_resource(&r, "TEMP NAME");

    if (sem_init(&t1, 0, 0) != 0) {
    	fprintf(stderr, "Failed to initialize semaphore: t1\n");
    	exit(1);
    }
    if (sem_init(&t2, 0, 1) != 0) {
    	fprintf(stderr, "Failed to initialize semaphore: t2\n");
    	exit(1);
    }
    if (sem_init(&mutex, 0, 1) != 0) {
    	fprintf(stderr, "Failed to initialize semaphore: mutex\n");
    	exit(1);
    }

}


void join_meetup(char *value, int len) {
    

    sem_wait(&mutex);

    if (count == 0 && meet == MEET_FIRST) { write_resource(&r, value, len); }

    if (++count == num) {
    	
    	if (meet == MEET_LAST) { write_resource(&r, value, len); }

    	sem_wait(&t2);
    	sem_post(&t1);
    }

    sem_post(&mutex);

    sem_wait(&t1);
    sem_post(&t1);

    sem_wait(&mutex);

    read_resource(&r, value, len);

    if (--count == 0) {
    	sem_wait(&t1);
    	sem_post(&t2);

    }

    sem_post(&mutex);
   	sem_wait(&t2);
    sem_post(&t2);

}
