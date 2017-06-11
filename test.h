/*
test.c contains the logic that runs tests within watzbench. 

additional information is available in the c file.
*/

#ifndef WATZBENCH_TEST_H
#define WATZBENCH_TEST_H
#include "api.h"
#include "common.h"
#include "contiki.h"

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

a report function should also be specified allowing the test to output
data
*/
struct Test{
    struct API* api;
    char* name;
    int start_time;
    int completion_time;
    int(*prepare)(void);
    int(*run)(void);
    int(*teardown)(void);
    char*(*report)(void);
};

struct Test* new_test(
    char*,
    int(*prepare_func)(void),
    int(*run_func)(void),
    int(*teardown_func)(void),
    char*(*report_func)(void)
);

void run_test(struct API*, struct Test*);
void init_test();
void cleanup_test();

#endif //WATZBENCH_TEST_H