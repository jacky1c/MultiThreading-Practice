/*
This program creates a semaphore and passes it to a number of threads.
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Header.h"


pthread_t *tids;
int n; // number of threads
int progress = 0; // it counts how many threads has finished work
void *threadout (void *args); // defined in threadFunction.c


int main (int argc, char *argv []){
    int error;
    int i;
    //sem_t sem;
    thread_arguments args;
    
    if (argc != 4){
        fprintf (stderr, "Usage: %s input_file_name_prefix number_of_threads output_file_name\n", argv [0]);
        return 1;
    }
    
    /******************* Construct parameters *******************/
    args.file_name_prefix = argv[1];
    args.out_put_file = argv[3];
    
    /******************* Allocate space for thread ids *******************/
    n = atoi (argv[2]);
    tids = (pthread_t *) calloc (n, sizeof (pthread_t));
    if (tids == NULL){
        perror ("ERROR: Failed to allocate memory for thread IDs");
        return 1;
    }
    
    /******************* Initialize semaphore *******************/
    if (sem_init (&(args.sem), 0, 1) == -1){
        perror ("ERROR: Failed to initialize semaphore");
        return 1;
    }

    /******************* Create threads **********************************/
    for (i = 0; i < n; i++){
        if (error = pthread_create (tids + i, NULL, threadout, &args)){
            fprintf (stderr, "ERROR: Failed to create thread:%s\n", strerror (error));
            return 1;
        }
    }

    /******************* Wait for threads to terminate *******************/
    for (i = 0; i < n; i++){
        if (error = pthread_join (tids [i], NULL)){
            fprintf (stderr, "ERROR: Failed to join thread:%s\n", strerror (error));
            return 1;
        }
    }

    /******************* Destroy thread **********************************/
    if (sem_destroy (&(args.sem)) == -1){
        perror ("ERROR: Failed to destroy semaphore");
        return 1;
    }

    fprintf (stdout, "This is tid 1 from pid %ld\n", (long) getpid ());

    return 0;
}
