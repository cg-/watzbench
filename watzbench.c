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

const int MAX_FILES = 100; // The maximum number of files to create
const int MAX_FILENAME_SIZE = 10; // Max number of characters in a filename (size of buffer)
const int MAX_WRITE_BYTES = 4096; // Max filesize to write
const int DEBUGGING_ENABLED = 1; // Debugging messages

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


    for(int i = 0; i < 100; i++){
        run_test(CFS, FileMetaDataCreate);
        printf("start: %u, stop: %u, time: %u\n", (uint)FileMetaDataCreate->start_time,(uint)FileMetaDataCreate->completion_time,((uint)FileMetaDataCreate->completion_time - (uint)FileMetaDataCreate->start_time));
    }


    cleanup();
    PROCESS_END();
}