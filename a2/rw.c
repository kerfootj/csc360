/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "rw.h"
#include "resource.h"

// Reference:
// https://connex.csc.uvic.ca/access/content/group/3b0d6b23-22bc-4a1a-892c-cd3e67333ceb/Lectures/02_concurrency_DRAFT.pdf

/*
 * Declarations for reader-writer shared variables -- plus concurrency-control
 * variables -- must START here.
 */

static resource_t data;

pthread_mutex_t m;
pthread_cond_t writersQ;
pthread_cond_t readersQ;

int readers;
int writers;
int active_writers;

void initialize_readers_writer() {
   
    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */

	readers = 0;
	writers = 0;
	active_writers = 0;

	if (pthread_mutex_init(&m, NULL) !=0 ) {
		fprintf(stderr, "Failed to initialize mutex: m\n");
		exit(0);
	}
	if (pthread_cond_init(&writersQ, NULL) != 0) {
		fprintf(stderr, "Failed to initialize condition variable: writersQ\n");
		exit(0);
	}
	if (pthread_cond_init(&readersQ, NULL) != 0) {
		fprintf(stderr, "Failed to initialize condition variable: readersQ\n");
		exit(0);
	}
}

void rw_read(char *value, int len) {
    
    pthread_mutex_lock(&m);

    // If writers then wait
    while (!(writers == 0)) {
		pthread_cond_wait(&readersQ, &m);
	}
	readers++;

	pthread_mutex_unlock(&m);

	read_resource(&data, value, len);

	pthread_mutex_lock(&m);

	// If no readers remaining signal writers
	if (--readers == 0) {
		pthread_cond_signal(&writersQ);
	}
	pthread_mutex_unlock(&m);
}

void rw_write(char *value, int len) {
    
    pthread_mutex_lock(&m);

    // Indicate wanting to write and wait if already reading or writing
    writers++;
   	while (!((readers == 0) && (active_writers == 0))) { 
   		pthread_cond_wait(&writersQ, &m);
   	}

   	active_writers++;

   	pthread_mutex_unlock(&m);

   	write_resource(&data, value, len);

   	pthread_mutex_lock(&m);

   	// If writers waiting signal writer else brodcast readers
   	writers--;
   	active_writers--;
   	if (writers) {
   		pthread_cond_signal(&writersQ);
   	} else {
   		pthread_cond_broadcast(&readersQ);
   	}
   	pthread_mutex_unlock(&m);
}