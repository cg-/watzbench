/*
api.c contains the interface between benchmarking tests and filesystems. this 
is what allows watzbench to perform the same tests across various filesystems 
(since in contiki filesystems are usually implemented with different functions 
that should be called by programs).

to create a new api, functions needs to be created that do the following:
- create a file
- delete a file
- create a directory
- delete a directory

once they have been created, a new API struct should be created with pointers 
to these functions.

at the bottom of this file is the init_api function, which sets everything up.
any new apis created will also need to be added to the init_api function.
*/
#include "api.h"

/*
new_api is a constructor for the API struct. function pointers need to be set 
to allow watzbench to run its tests>
*/
struct API* new_api(
        int(*create_f)(char*), 
        int(*delete_f)(char*), 
        int(*create_d)(char*),
        int(*delete_d)(char*)
    ){
    void* t = malloc(sizeof(struct API));
    struct API* api_ptr = (struct API*)t;
    api_ptr->create_file = create_f;
    api_ptr->delete_file = delete_f;
    api_ptr->create_dir = create_d;
    api_ptr->delete_dir = delete_d;
    return api_ptr;
}

void free_api(struct API* api){
    free(api);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
CFS Functions

The below functions define the interface with the contiki posix file system
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
struct API* CFS; // Pointer to CFS API

int cfs_create_file(char* name){
    int fd = cfs_open(name, CFS_WRITE);
    if (fd == -1){
        return -1;
    }
    cfs_close(fd);

    printf("create cfs file called. [%s]\n", name);
    return fd;
}

int cfs_delete_file(char* name){
    int err = cfs_remove(name);
    if (err == -1){
        return -1;
    }

    printf("delete cfs file called. [%s]\n", name);
    return 0;
}

int cfs_create_dir(char* name){
    struct cfs_dir* dirp = NULL;
    int err = cfs_opendir(dirp, name);
    if(err == -1){
        return -1;
    }
    cfs_closedir(dirp);
    printf("create cfs dir called. [%s]\n", name);
    return 0;
}

int cfs_delete_dir(char* name){
    int err = cfs_remove(name);
    if (err == -1){
        return -1;
    }

    printf("delete directory called. [%s]\n", name);
    return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Other Functions

The functions below are other misc functions needed by the API
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
init_api is calle when the program starts. this is where we call the API 
constructors for defined filesystems
*/
void init_api(){
    CFS = new_api(cfs_create_file, cfs_delete_file, cfs_create_dir, cfs_delete_dir);
}

/*
cleanup_api is calle before the program exits. this is where we call the API 
destructors for defined filesystems
*/
void cleanup_api(){
    free_api(CFS);
}