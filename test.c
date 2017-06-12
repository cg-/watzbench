/*
test.c contains the logic that runs tests within watzbench. 

tests should be created in such a way that they call functions from the 
provided api. when a test is run, the pointer to the api will be set in the 
test struct, and those functions will be used throughout the test. upon 
test completion, the pointer will be removed.
*/

#include "test.h"

/*
new_test is a constructor for the test. the various components of the test 
need to be specified as function pointers. additional information on the 
components is available in the header file struct definition.
*/
struct Test* new_test(
        char* test_name,
        int(*prepare_func)(struct Test* test),
        int(*run_func)(struct Test* test),
        int(*teardown_func)(struct Test* test)
    ){
    void* t = malloc(sizeof(struct Test));
    struct Test* test_ptr = (struct Test*)t;
    test_ptr->api = NULL;
    test_ptr->params = NULL;
    test_ptr->name = test_name;
    test_ptr->start_time = 0;
    test_ptr->completion_time = 0;
    test_ptr->prepare = prepare_func;
    test_ptr->run = run_func;
    test_ptr->teardown = teardown_func;
    return test_ptr;
}

/*
free_test is a destructor for the Test struct
*/
void free_test(struct Test* test){
    if(test->params != NULL){
        free_test_params(test->params);
        test->params = NULL;
    }
    free(test);
}

struct TestParams* new_test_params(){
    void* t = malloc(sizeof(struct TestParams));
    struct TestParams* test_params_ptr = (struct TestParams*)t;
    test_params_ptr->rands = NULL;
    test_params_ptr->filename = NULL;
    test_params_ptr->buffer = NULL;
    test_params_ptr->count = 0;
    test_params_ptr->fd = -1;
    test_params_ptr->sizes = NULL;
    return test_params_ptr;
}

/*
free_test is a destructor for the Test struct
*/
void free_test_params(struct TestParams* params){
    if(params->buffer != NULL){
        free(params->buffer);
    }
    free(params);
}

/*
run_test actually executes the test.
*/
void run_test(struct API* api_ptr, struct Test* test){
    test->api = api_ptr;
    test->api->init();
    int err = test->prepare(test);
    check(err, "error in prepare function", TRUE);
    test->start_time = clock_time();
    err = test->run(test);
    test->completion_time = clock_time();
    check(err, "error in test function", TRUE);
    err = test->teardown(test);
    check(err, "error in teardown function", TRUE);
    test->api = NULL;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Tests

The functions below define the various benchmarks WatzBench supports
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// FILE METADATA
// creates a large number of files

// CREATE FILES
int file_metadata_create_test_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->count = MAX_FILES;
    return 0;
}

int file_metadata_create_test_run(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->create_file(filename);
    }
    return 0;
}

int file_metadata_create_test_cleanup(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->delete_file(filename);
    }

    free_test_params(test->params);
    test->params = NULL;
    return 0;
}

// DELETE FILES
int file_metadata_delete_test_prepare(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    test->params = new_test_params();
    test->params->count = MAX_FILES;
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->create_file(filename);
    }
    return 0;
}

int file_metadata_delete_test_run(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->delete_file(filename);
    }
    return 0;
}

int file_metadata_delete_test_cleanup(struct Test* test){
    free_test_params(test->params);
    test->params = NULL;
    return 0;
}

// THROUGHPUT
// SEQ READS
int throughout_seq_read_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->filename = "WATZ";
    void* t = malloc(sizeof(char)*MAX_WRITE_BYTES);
    test->params->buffer = (char*)t;
    test->api->create_file(test->params->filename);
    test->params->fd = test->api->open_get_fd(test->params->filename);
    test->api->write_at(test->params->fd, 0, sizeof(test->params->buffer), test->params->buffer);
    return 0;
}

int throughout_seq_read_run(struct Test* test){
    test->api->read_at(test->params->fd, 0, sizeof(test->params->buffer), test->params->buffer);
    return 0;
}

int throughout_seq_read_cleanup(struct Test* test){
    test->api->close_fd(test->params->fd);
    test->api->delete_file(test->params->filename);
    free_test_params(test->params);
    test->params = NULL;
    return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Other Functions

The functions below are other misc functions needed by the testing framework
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
struct Test* FileMetaDataCreate;
struct Test* FileMetaDataDelete;
struct Test* ThroughputSeqRead;

/*
init_test is called when the program starts.
*/
void init_test(){
    FileMetaDataCreate= new_test(
        "File Metadata Create File Test",
        file_metadata_create_test_prepare,
        file_metadata_create_test_run,
        file_metadata_create_test_cleanup
    );
    FileMetaDataDelete= new_test(
        "File Metadata Delete File Test",
        file_metadata_delete_test_prepare,
        file_metadata_delete_test_run,
        file_metadata_delete_test_cleanup
    );
    ThroughputSeqRead= new_test(
        "Throughput Sequential Read Test",
        throughout_seq_read_prepare,
        throughout_seq_read_run,
        throughout_seq_read_cleanup
    );
    return;
}

/*
cleanup_test is called before the program exits
*/
void cleanup_test(){
    free_test(FileMetaDataCreate);
    free_test(FileMetaDataDelete);
    free_test(ThroughputSeqRead);
    return;
}
