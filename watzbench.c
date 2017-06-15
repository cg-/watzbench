/*
WatzBench is a storage system benchmarking tool developed for the
Contiki operating system.

components:
api.c/h: the interface between watzbench and various filesystems
test.c/h: tests defined using the interfaces provided by the API
common.c/h: useful functions used throughout watzbench 

UCSC - CMPE259 - Spring 2017 - Cole Grim
 */

#include "contiki.h"
#include "api.h"
#include "test.h"
#include "common.h"

// Testing Parameters
const int FILES_TO_CREATE = 100; // The maximum number of files to create
const int MAX_FILENAME_SIZE = 10; // Max number of characters in a filename (size of buffer)
int WRITE_BYTES = 1024; // Max filesize to write
int BUFFER = 128;   // Size of buffer

// Program Options
const int DEBUGGING_ENABLED = 1; // Debugging messages
const int POWER_TESTS = 1; // enable or disable power consumption tests

void init(){
    log_info("program starting");
    init_api();
    init_test();
}

void cleanup(){
    cleanup_test();
    cleanup_api();
    log_info("program exiting");
}

PROCESS(watzbench_process, "Watzbench process");
AUTOSTART_PROCESSES(&watzbench_process);
PROCESS_THREAD(watzbench_process, ev, data) {
    PROCESS_BEGIN();
    init();

    printf("Beginning WatzBench.\n\n");
    printf("Important Device Information:\n");
    printf("1 second = %lu ticks.\n\n", CLOCK_SECOND);

    /* Example Usage */
    WRITE_BYTES = 1024; // Write 1K files
    BUFFER = 128;       // With a buffer of 128B
    run_test(Coffee, ThroughputRandWrite);

    cleanup();
    PROCESS_END();
}