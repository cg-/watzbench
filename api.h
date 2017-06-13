/*
api.c contains the interface between benchmarking tests and filesystems. this 
is what allows watzbench to perform the same tests across various filesystems 
(since in contiki filesystems are usually implemented with different functions 
that should be called by programs).

additional information and descriptions are in the c file
*/

#ifndef WATZBENCH_API_H
#define WATZBENCH_API_H
#include <stdlib.h>
#include <stdio.h>
#include <cfs/cfs.h>
#include <cfs/cfs-coffee.h>

#include "common.h"

/*
supported filesystems

once an API has been created, add it as an extern here to allow watzbench 
to use it.
*/
extern struct API* CFS;
extern struct API* Coffee;

/*
API is a struct that will control the interface with the underlying
filesystem
*/
struct API{
    void (*init)();
    int (*create_file)(char*);
    int (*delete_file)(char*);
    int (*create_dir)(char*);
    int (*delete_dir)(char*);
    int (*open_get_fd)(char*);
    int (*write_at)(int, int, int, char*);
    int (*read_at)(int, int, int, char*);
    int (*close_fd)(int);
};

struct API* new_api(
    void (*init)(),
    int(*create_f)(char*), 
    int(*delete_f)(char*), 
    int(*create_d)(char*),
    int(*delete_d)(char*),
    int (*open_get_fd)(char*),
    int (*write_at)(int, int, int, char*),
    int (*read_at)(int, int, int, char*),
    int (*close_fd)(int)
);

void free_api(struct API*);

void init_api();
void cleanup_api();
#endif //WATZBENCH_API_H