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
int count;
int meet;

resource_t r;

sem_t t1;
sem_t t2;
sem_t mutex;
sem_t barrier;

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
     if (sem_init(&barrier, 0, num) != 0) {
    	fprintf(stderr, "Failed to initialize semaphore: barrier\n");
    	exit(1);
    }
}

// http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
void join_meetup(char *value, int len) {
    
    // Wait for current meetup to finish if full
    sem_wait(&barrier);

    /* Section 1 */
    sem_wait(&mutex); 

    // Add waiting thread
    count += 1;

    // If MEET_FIRST save the first thread's keyword 
    if (count == 1 && meet == MEET_FIRST) { write_resource(&r, value, len); }
  	
  	// Once meetup is full release threads
   	if (count == num) {
    	
    	// If MEET_LAST save the last thread's keyword
    	if (meet == MEET_LAST) { write_resource(&r, value, len); }

    	sem_wait(&t2); // Unlock section 1
    	sem_post(&t1); // Lock section 2
    }
    sem_post(&mutex);
    /* End Section 1 */

    // Wait for meetup to fill
    sem_wait(&t1);
    sem_post(&t1);

    /* Section 2 */
    sem_wait(&mutex); 

    // Remove thread from waiting and sync keyword
    count -= 1;
    read_resource(&r, value, len);

    if (count == 0) {
    	sem_wait(&t1); // Lock section 1
    	sem_post(&t2); // Unlock section 2
    }

    sem_post(&mutex);
    /* End Section 2*/

    // Sync threads release
   	sem_wait(&t2);
    sem_post(&t2);

    // Allow next batch to begin meetup
    sem_post(&barrier);
}
