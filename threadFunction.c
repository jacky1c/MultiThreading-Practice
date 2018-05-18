/*
A thread function with a critical section protected by a semaphore passed as its parameter.
*/

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Header.h"

#define TEN_MILLION 10000000L
#define BUFFER_SIZE 1024
#define FILE_NAME_SIZE 128
#define MAX_LINE_SIZE 1024
extern pthread_t *tids;
extern int n;
extern int progress;

int write_n (int fd, char *ptr, int n_bytes);
int read_line (int fd, char *ptr, int line_size);
int read_n (int fd, char *ptr, int n_bytes);

void *threadout (void *args){
    thread_arguments * arguments;
    sem_t *semp;
    char * file_name_prefix;
    char buffer [BUFFER_SIZE];
    char infile [FILE_NAME_SIZE];
    char outfile [FILE_NAME_SIZE];
    int out_filedes;
    FILE * in_filedes;
    char *c;
    char file_line [MAX_LINE_SIZE];
    int i, num_of_chars;
    
    
    pthread_t tid = pthread_self ();
    struct timespec sleeptime;
    
    
    //*** Initialize sleep time
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = TEN_MILLION;
    
    //*** Get thread id (starting from 0)
    /*
     This for loop converts a thread id into an integer. It works on both Hercules and the Linux machines in CL115. It solves the problem of printing thread ids when they are implemented as structs (such on the Linux machines in CL115). In this way, thread ids are handled consistently when printed from the different POSIX implementations.
     */
    for (i = 0; i < n; i ++){
        if (pthread_equal (tid, tids [i])){
            break;
        }
    }
    
    //*** Get sempahore, infile, and outfile from arguments struct
    arguments = (thread_arguments *) args;
    semp = &(arguments->sem);
    file_name_prefix = arguments->file_name_prefix;
    // Append thread id to file_name_prefix
    sprintf(infile, "%s%d", file_name_prefix, (int) i + 1);
    sprintf(outfile, "%s", arguments->out_put_file);
    
    //*** Open infile corresponded to thread id
    in_filedes = fopen(infile, "rb");
    if(in_filedes == NULL){
        fprintf (stderr, "ERROR: Thread failed to open input file\n");
        return NULL;
    }
    
    //*** Open outfile
    if((out_filedes = open(outfile, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
        printf("ERROR: Failed to open output file\n");
        return NULL;
    }

    //*** Construct message for thread to print to screen
    num_of_chars = snprintf (buffer, BUFFER_SIZE, "This is tid %d from pid %ld. I finished my work.\n", (int) i + 2, (long) getpid ());
    if (num_of_chars < 0){
        fprintf (stderr, "ERROR: Thread failed to write in buffer\n");
        return NULL;
    }
    c = buffer;

    /***************** Thread waits for its turn **********************/
    while (progress != i){
        // wait for other thread to finish
    }
    
    /******************* Lock *****************************************/
    while (sem_wait (semp) == -1){
        if(errno != EINTR){
            fprintf (stderr, "ERROR: Thread failed to lock semaphore\n");
            return NULL;
        }
    }

    /******************* Critical Section *****************************/
    //*** Read line from infile and write to outfile
    while (fgets (file_line, MAX_LINE_SIZE, in_filedes) != (char *) NULL){
        if ((num_of_chars = write_n(out_filedes, file_line, (int)strlen(file_line))) != (int)strlen(file_line)){
                    fprintf (stderr, "ERROR: Thread failed to write to file\n");
                    return NULL;
                }
    }
    
    //*** Write thread's message to screen
    while (*c != '\0'){
        fputc (*c, stdout);
        c++;
        nanosleep (&sleeptime, NULL);
    }
    progress ++;
    
    /******************* Unlock ***************************************/
    if (sem_post (semp) == -1){
        fprintf (stderr, "ERROR: Thread failed to unlock semaphore\n");
    }

    return NULL;
}






/***************** Supporting functions *********************/
// Write n bytes of characters in ptr to file descriptor.
// Called by threadout
int write_n (int fd, char *ptr, int n_bytes){
    int n_left;
    int n_written;
    
    n_left = n_bytes;
    
    while (n_left > 0){
        n_written = write (fd, ptr, n_left);
        if (n_written <= 0){
            return (n_written);
        }
        n_left = n_left - n_written;
        ptr = ptr + n_written;
    }
    
    return (n_bytes - n_left);
}


/*
 Read from file descriptor up to [line_size] characters or when '\0' is reached, one at a time.
 Store characters to ptr
 Return the number of characters read in (including '\0')
 ptr is also returned
 */
// Called by threadout
int read_line (int fd, char *ptr, int line_size){
    int n;
    int rc;
    char c;
    
    for (n = 1; n <= line_size; n++){
        // if we successfully get 1 character, append it to the end of ptr
        // if the character is '\0', return n
        if ((rc = read_n (fd, &c, 1)) == 1){
            *ptr++ = c;
            if (c == '\0'){
                break;
            }
        }
        // if we get 0 character by calling read_n (fd, &c, 1)
        else if (rc == 0){
            // if this is the 1st loop, we get nothing
            if (n == 1){
                return (0);
            }
            // if this is not the 1st loop, we reach the end of string in file descriptor
            // string length is less than line_size and the string doesn't end with '0'
            // append '\0' to the end of the string
            else{
                *ptr++ = '\0';
                break;
            }
        }
        // error
        else{
            return (-1);
        }
    }
    
    return (n);
}


// Called by read_line
// Read n bytes of characters from file descriptor and store in ptr
int read_n (int fd, char *ptr, int n_bytes){
    int n_left;
    int n_read;
    
    n_left = n_bytes;
    
    while (n_left > 0){
        n_read = read (fd, ptr, n_left);
        if (n_read < 0){
            return (n_read);
        }
        else if (n_read == 0){
            break;
        }
        n_left = n_left - n_read;
        ptr = ptr + n_read;
    }
    
    return (n_bytes - n_left);
}
