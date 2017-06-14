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
int BUFFER = 128;

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

    // Verification
    /*
    printf("VerifyModifyInitial\n");
    for(int i = 0;i < 10;i++){
        run_test(Coffee, VerifyModifyInitial);
    }
    printf("VerifyModifySub\n");
    for(int i = 0;i < 10;i++){
        run_test(Coffee, VerifyModifySub);
    }
    printf("VerifyOpenUncached\n");
    for(int i = 0;i < 10;i++){
        run_test(Coffee, VerifyOpenUncached);
    }
    printf("VerifyOpenCached\n");
    for(int i = 0;i < 10;i++){
        run_test(Coffee, VerifyOpenCached);
    }
    */

/*
    // Microbenchmarks
    printf("CreateFile\n");
    for(int i = 0;i < 1;i++){
        run_test(Coffee, FileMetaDataCreate);
    }
    printf("CreateFile\n");
    for(int i = 0;i < 1;i++){
        run_test(CFS, FileMetaDataCreate);
    }
/*

    printf("DeleteFile\n");
    for(int i = 0;i < 100;i++){
        run_test(Coffee, FileMetaDataDelete);
    }

    printf("OpenFile\n");
    for(int i = 0;i < 100;i++){
        run_test(Coffee, FileMetaDataOpen);
    }
    */
    /*
    int bufsize[] = {16, 32, 64, 128, 256, 512};
    int filesize[] = {512, 1024};

    for(int bufsizei = 0; bufsizei < 6; bufsizei++){
        BUFFER = bufsize[bufsizei];
        printf("Microbenchmarks\n");
        printf("BUFFER: %d\n", BUFFER);
        for(int filesizei = 0; filesizei < 2; filesizei++){
            */
            //WRITE_BYTES = filesize[filesizei];
            BUFFER = 128;
            WRITE_BYTES = 512;
            printf("BUFFER: %d\n", BUFFER);
            printf("WRITEBYTES: %d\n", WRITE_BYTES);

            printf("SeqRead\n");
            for(int i = 0;i < 5;i++){
                run_test(Coffee, ThroughputSeqRead);
            }

            printf("SeqWrite\n");
            for(int i = 0;i < 5;i++){
                run_test(Coffee, ThroughputSeqWrite);
            }

            printf("RandRead\n");
            for(int i = 0;i < 5;i++){
                run_test(Coffee, ThroughputRandRead);
            }

            printf("RandWrite\n");
            for(int i = 0;i < 5;i++){
                run_test(Coffee, ThroughputRandWrite);
            }
            /*
        }
    }
    */

    /*
    for(int bufsizei = 0; bufsizei < 6; bufsizei++){
        BUFFER = bufsize[bufsizei];
        printf("Macrobenchmarks\n");
        printf("BUFFER: %d\n", BUFFER);
        for(int filesizei = 0; filesizei < 2; filesizei++){
            WRITE_BYTES = filesize[filesizei];
            printf("WRITEBYTES: %d\n", WRITE_BYTES);
            BUFFER = 16;
            WRITE_BYTES = 128;
            printf("BUFFER: %d\n", BUFFER);
            printf("WRITEBYTES: %d\n", WRITE_BYTES);

            printf("Archival\n");
            for(int i = 0;i < 10;i++){
                run_test(Coffee, ArchivalStorage);
            }

            printf("Archival Query\n");
            for(int i = 0;i < 10;i++){
                run_test(Coffee, ArchivalStorageAndQuery);
            }
        }
    }
        */

    for(;;){
        printf("DONE\n");
        run_test(Coffee, ThroughputRandWrite);
    }

/*
    // Macrobenchmarks
    for(int i = 0; i < 10; i++){
        run_test(CFS, ArchivalStorageAndQuery);
    }
*/
    /*
    run_test(Coffee, ArchivalStorageAndQuery);
    run_test(Coffee, SignalProcessing);
    run_test(Coffee, NetworkRouting);
    run_test(Coffee, DebuggingLogs);
    run_test(Coffee, Calibration);
    */
    cleanup();
    PROCESS_END();
}