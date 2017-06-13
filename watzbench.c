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
const int WRITE_BYTES = 4096; // Max filesize to write
const int RAND_READ_SIZE = 128; // Max filesize to write
const int RAND_WRITE_SIZE = 128; // Max filesize to write
const int WRITE_BUFFER = 128;

// Program Options
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

    // Verification
    for(int i = 0;i < 10;i++){
    run_test(Coffee, VerifyModifySub);
    }
    run_test(Coffee, VerifyOpenUncached);
        run_test(Coffee, VerifyOpenCached);

    // Microbenchmarks
    /*
    run_test(Coffee, FileMetaDataCreate);
    run_test(Coffee, FileMetaDataDelete);
    run_test(Coffee, FileMetaDataOpen);
    run_test(Coffee, ThroughputSeqRead);
    run_test(Coffee, ThroughputSeqWrite);
    run_test(Coffee, ThroughputRandRead);
    run_test(Coffee, ThroughputRandWrite);
    */

    /*
    // Macrobenchmarks
    run_test(Coffee, ArchivalStorage);
    run_test(Coffee, ArchivalStorageAndQuery);
    run_test(Coffee, SignalProcessing);
    run_test(Coffee, NetworkRouting);
    run_test(Coffee, DebuggingLogs);
    run_test(Coffee, Calibration);
    */
    cleanup();
    PROCESS_END();
}