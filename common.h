/*
common.h contains useful functions that are used throughout watzbench
descriptions are in the c file.
*/

#ifndef WATZBENCH_COMMON_H
#define WATZBENCH_COMMON_H

#include <stdio.h>
#define TRUE 1
#define FALSE 0

extern const int DEBUGGING_ENABLED;

void log_info(char*);
void log_error(char*);
void check(int err, char* msg, int fatal);

#endif //WATZBENCH_COMMON_H