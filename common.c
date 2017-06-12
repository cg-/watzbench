/*
common.c contains useful functions that are used throughout watzbench
*/
#include "common.h"

/*
log_info is a debugging function that will display messages to stdout
if DEBUGGING_ENABLED is set to 1.
 */
void log_info(char* msg){
    if (DEBUGGING_ENABLED == 1){
        printf("info: %s\n", msg);
    }
    return;
}

/*
log_error is a debugging message that will print an error message to 
stdout.

todo: change to stderr
*/
void log_error(char* msg){
    printf("ERROR: %s\n", msg);
    return;
}

/*
check is a utility function that will check a returned error value.
if it's -1 (what contiki and watzbench usually return if a function 
failed) it will print an error message. the fatal parameter tells 
the program to exit with an error code if an error was found.
*/
void check(int err, char* msg, int fatal){
    if(err == -1){
        log_error(msg);
        if(fatal == 1){
            exit(-1);
        }
    }
}
