/*
test.c contains the logic that runs tests within watzbench. 

additional information is available in the c file.
*/

#ifndef WATZBENCH_TEST_H
#define WATZBENCH_TEST_H
#include "api.h"
#include "common.h"
#include "contiki.h"

extern struct Test* FileMetaDataCreate;
extern struct Test* FileMetaDataDelete;
extern struct Test* ThroughputSeqRead;
extern const int MAX_FILES;
extern const int MAX_FILENAME_SIZE;
extern const int MAX_WRITE_BYTES;

/*
Test is a struct that represents the various parts of a benchmarking
test. initially the api pointer is left undefined. when a call to 
run_test is made, the API must be specified.

tests have 3 major parts--
 - prepare: this is anything that should be done beforehand to set up
   the test.
 - run: the test itself. this needs to be implemented only using functions
   provided by the API.
 - teardown: any cleanup.

the TestParams structure can be modified to allow paramaters to be passed 
to the tests.
*/
struct Test{
    struct API* api;
    struct TestParams* params;
    char* name;
    clock_time_t start_time;
    clock_time_t completion_time;
    int(*prepare)(struct Test* test);
    int(*run)(struct Test* test);
    int(*teardown)(struct Test* test);
};

struct Test* new_test(
    char*,
    int(*prepare_func)(struct Test* test),
    int(*run_func)(struct Test* test),
    int(*teardown_func)(struct Test* test)
);
void free_test(struct Test* test);
void run_test(struct API*, struct Test*);

struct TestParams{
    char* filename;
    char* buffer;
    int count;
    int fd;
    int* sizes;
    int* rands;
};
struct TestParams* new_test_params();
void free_test_params(struct TestParams* test_params);

void init_test();
void cleanup_test();

#endif //WATZBENCH_TEST_H