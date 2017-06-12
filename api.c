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
        void(*init_func)(), 
        int(*create_file_func)(char*), 
        int(*delete_file_func)(char*), 
        int(*create_dir_func)(char*),
        int(*delete_dir_func)(char*),
        int(*open_get_fd_func)(char*),
        int(*write_at_func)(int, int, int, char*),
        int(*read_at_func)(int, int, int, char*),
        int(*close_fd_func)(int)
    ){
    void* t = malloc(sizeof(struct API));
    struct API* api_ptr = (struct API*)t;
    api_ptr->init = init_func;
    api_ptr->create_file = create_file_func;
    api_ptr->delete_file = delete_file_func;
    api_ptr->create_dir = create_dir_func;
    api_ptr->delete_dir = delete_dir_func;
    api_ptr->open_get_fd = open_get_fd_func;
    api_ptr->write_at = write_at_func;
    api_ptr->read_at = read_at_func;
    api_ptr->close_fd = close_fd_func;
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

void cfs_init(){

}

int cfs_create_file(char* name){
    int fd = cfs_open(name, CFS_WRITE);
    if (fd == -1){
        return -1;
    }
    cfs_close(fd);

    return fd;
}

int cfs_delete_file(char* name){
    int err = cfs_remove(name);
    if (err == -1){
        return -1;
    }

    return 0;
}

int cfs_create_dir(char* name){
    struct cfs_dir* dirp = NULL;
    int err = cfs_opendir(dirp, name);
    if(err == -1){
        return -1;
    }
    cfs_closedir(dirp);
    return 0;
}

int cfs_delete_dir(char* name){
    int err = cfs_remove(name);
    if (err == -1){
        return -1;
    }

    return 0;
}

int cfs_open_get_fd(char* name){
    return cfs_open(name, CFS_READ | CFS_APPEND);
}

int cfs_write_at(int fd, int start_pos, int bytes, char* buf){
    cfs_seek(fd, start_pos, CFS_SEEK_SET);
    cfs_write(fd, buf, sizeof(buf));
    return 0;
}

int cfs_read_at(int fd, int start_pos, int bytes, char* buf){
    cfs_seek(fd, start_pos, CFS_SEEK_SET);
    cfs_read(fd, buf, sizeof(buf));
    return 0;
}

int cfs_close_fd(int fd){
    cfs_close(fd);
    return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
CoffeeFS Functions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
struct API* Coffee; // Pointer to CFS API

void coffee_init(){
    cfs_coffee_format();
}

int coffee_create_file(char* name){
    int fd = cfs_open(name, CFS_WRITE);
    if (fd == -1){
        return -1;
    }
    cfs_close(fd);

    return fd;
}

int coffee_delete_file(char* name){
    int err = cfs_remove(name);
    if (err == -1){
        return -1;
    }

    return 0;
}

int coffee_create_dir(char* name){
    struct cfs_dir* dirp = NULL;
    int err = cfs_opendir(dirp, name);
    if(err == -1){
        return -1;
    }
    cfs_closedir(dirp);
    return 0;
}

int coffee_delete_dir(char* name){
    int err = cfs_remove(name);
    if (err == -1){
        return -1;
    }

    return 0;
}

int coffee_open_get_fd(char* name){
    return cfs_open(name, CFS_READ | CFS_APPEND);
}

int coffee_write_at(int fd, int start_pos, int bytes, char* buf){
    cfs_seek(fd, start_pos, CFS_SEEK_SET);
    cfs_write(fd, buf, sizeof(buf));
    return 0;
}

int coffee_read_at(int fd, int start_pos, int bytes, char* buf){
    cfs_seek(fd, start_pos, CFS_SEEK_SET);
    cfs_read(fd, buf, sizeof(buf));
    return 0;
}

int coffee_close_fd(int fd){
    cfs_close(fd);
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
    CFS = new_api(
        cfs_init,
        cfs_create_file, 
        cfs_delete_file, 
        cfs_create_dir, 
        cfs_delete_dir,
        cfs_open_get_fd,
        cfs_write_at,
        cfs_read_at,
        cfs_close_fd
        );

    Coffee = new_api(
        coffee_init,
        coffee_create_file, 
        coffee_delete_file, 
        coffee_create_dir, 
        coffee_delete_dir,
        coffee_open_get_fd,
        coffee_write_at,
        coffee_read_at,
        coffee_close_fd
        );
}

/*
cleanup_api is calle before the program exits. this is where we call the API 
destructors for defined filesystems
*/
void cleanup_api(){
    free_api(CFS);
    free_api(Coffee);
}