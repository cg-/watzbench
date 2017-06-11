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
        int(*prepare_func)(void),
        int(*run_func)(void),
        int(*teardown_func)(void),
        char*(*report_func)(void)
    ){
    void* t = malloc(sizeof(struct Test));
    struct Test* test_ptr = (struct Test*)t;
    test_ptr->name = test_name;
    test_ptr->start_time = 0;
    test_ptr->completion_time = 0;
    test_ptr->prepare = prepare_func;
    test_ptr->run = run_func;
    test_ptr->teardown = teardown_func;
    test_ptr->report = report_func;
    return test_ptr;
}

/*
run_test actually executes the test.
*/
void run_test(struct API* api_ptr, struct Test* test){
    test->api = api_ptr;
    int err = test->prepare();
    check(err, "error in prepare function", TRUE);
    int a = RTIMER_NOW();
    printf("!!!! %d !!!!\n\n", a);
    err = test->run();
    check(err, "error in test function", TRUE);
    err = test->teardown();
    check(err, "error in teardown function", TRUE);
    test->api = NULL;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Tests

The functions below define the various benchmarks WatzBench supports
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Other Functions

The functions below are other misc functions needed by the testing framework
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
init_test is called when the program starts.
*/
void init_test(){
    return;
}

/*
cleanup_test is called before the program exits
*/
void cleanup_test(){
    return;
}
