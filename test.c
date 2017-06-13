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
    test_params_ptr->fds = NULL;
    return test_params_ptr;
}

/*
free_test is a destructor for the Test structextern struct Test* FileMetaDataCreate;
extern struct Test* FileMetaDataDelete;
*/
void free_test_params(struct TestParams* params){
    if(params->buffer != NULL){
        free(params->buffer);
    }
    if(params->fds != NULL){
        free(params->fds);
    }
    if(params->rands != NULL){
        free(params->rands);
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
    printf("test: %s, start: %u, stop: %u, time: %u\n", test->name,(uint)test->start_time,(uint)test->completion_time,((uint)test->completion_time - (uint)test->start_time));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Tests

The functions below define the various benchmarks WatzBench supports
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/// VERIFICATION

// Open Uncached File
int verification_open_uncached_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->count = FILES_TO_CREATE;
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->create_file(filename);
    }
    return 0;
}

int verification_open_uncached_run(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    sprintf(filename, "%d", (random_rand() % test->params->count));
    test->api->open_get_fd(filename);
    return 0;
}

int verification_open_uncached_cleanup(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->delete_file(filename);
    }
    free_test_params(test->params);
    return 0;
}

// Open Cached File
int verification_open_cached_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->count = FILES_TO_CREATE;
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->create_file(filename);
    }
    sprintf(filename, "%d", (random_rand() % test->params->count));
    test->params->filename = filename;
    test->params->fd = test->api->open_get_fd(test->params->filename);
    test->api->close_fd(test->params->fd);
    return 0;
}

int verification_open_cached_run(struct Test* test){
    test->api->open_get_fd(test->params->filename);
    return 0;
}

int verification_open_cached_cleanup(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->delete_file(filename);
    }
    free_test_params(test->params);
    return 0;
}

// Initial File Modification
int verification_initial_modify_prepare(struct Test* test){
    test->params = new_test_params();

    void* t = malloc(WRITE_BYTES);
    test->params->buffer = (char*)t;

    test->params->count = FILES_TO_CREATE;
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->create_file(filename);
        int fd = test->api->open_get_fd(test->params->filename);
        int at = 0;
        while(at < WRITE_BYTES){
            if(WRITE_BYTES - at < WRITE_BUFFER){
                test->api->write_at(fd, 0, WRITE_BYTES - at, test->params->buffer);
                at = WRITE_BYTES;
            }else{
                test->api->write_at(fd, 0, WRITE_BUFFER, test->params->buffer);
                at += WRITE_BUFFER;
            }
        }
        test->api->close_fd(fd);
    }

    test->api->create_file("WATZ");
    test->params->fd = test->api->open_get_fd("WATZ");
    int at = 0;
    while(at < WRITE_BYTES){
        if(WRITE_BYTES - at < WRITE_BUFFER){
            test->api->write_at(test->params->fd, 0, WRITE_BYTES - at, test->params->buffer);
            at = WRITE_BYTES;
        }else{
            test->api->write_at(test->params->fd, 0, WRITE_BUFFER, test->params->buffer);
            at += WRITE_BUFFER;
        }
    }
    test->api->close_fd(test->params->fd);
    test->params->fd = test->api->open_get_fd("WATZ");
    return 0;
}

int verification_initial_modify_run(struct Test* test){
    int at = 0;
    while(at < WRITE_BYTES){
        if(WRITE_BYTES - at < WRITE_BUFFER){
            test->api->write_at(test->params->fd, 0, WRITE_BYTES - at, test->params->buffer);
            at = WRITE_BYTES;
        }else{
            test->api->write_at(test->params->fd, 0, WRITE_BUFFER, test->params->buffer);
            at += WRITE_BUFFER;
        }
    }
    return 0;
}

int verification_initial_modify_cleanup(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    test->api->close_fd(test->params->fd);
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->delete_file(filename);
    }
    free_test_params(test->params);
    test->api->delete_file("WATZ");
    return 0;
}

// Subsequent File Modification
int verification_sub_modify_prepare(struct Test* test){
    test->params = new_test_params();

    void* t = malloc(WRITE_BYTES);
    test->params->buffer = (char*)t;

    test->params->count = FILES_TO_CREATE;
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->create_file(filename);
        int fd = test->api->open_get_fd(test->params->filename);
        int at = 0;
        while(at < WRITE_BYTES){
            if(WRITE_BYTES - at < WRITE_BUFFER){
                test->api->write_at(fd, 0, WRITE_BYTES - at, test->params->buffer);
                at = WRITE_BYTES;
            }else{
                test->api->write_at(fd, 0, WRITE_BUFFER, test->params->buffer);
                at += WRITE_BUFFER;
            }
        }
        test->api->close_fd(fd);
    }

    test->api->create_file("WATZ");
    test->params->fd = test->api->open_get_fd("WATZ");
    int at = 0;
    while(at < WRITE_BYTES){
        if(WRITE_BYTES - at < WRITE_BUFFER){
            test->api->write_at(test->params->fd, 0, WRITE_BYTES - at, test->params->buffer);
            at = WRITE_BYTES;
        }else{
            test->api->write_at(test->params->fd, 0, WRITE_BUFFER, test->params->buffer);
            at += WRITE_BUFFER;
        }
    }
    test->api->close_fd(test->params->fd);
    test->params->fd = test->api->open_get_fd("WATZ");
    at = 0;
    while(at < WRITE_BYTES){
        if(WRITE_BYTES - at < WRITE_BUFFER){
            test->api->write_at(test->params->fd, WRITE_BYTES, WRITE_BYTES - at, test->params->buffer);
            at = WRITE_BYTES;
        }else{
            test->api->write_at(test->params->fd, WRITE_BYTES, WRITE_BUFFER, test->params->buffer);
            at += WRITE_BUFFER;
        }
    }
    test->api->close_fd(test->params->fd);
    test->params->fd = test->api->open_get_fd("WATZ");
    return 0;
}

int verification_sub_modify_run(struct Test* test){
    int at = 0;
    while(at < WRITE_BYTES){
        if(WRITE_BYTES - at < WRITE_BUFFER){
            test->api->write_at(test->params->fd, 0, WRITE_BYTES - at, test->params->buffer);
            at = WRITE_BYTES;
        }else{
            test->api->write_at(test->params->fd, 0, WRITE_BUFFER, test->params->buffer);
            at += WRITE_BUFFER;
        }
    }
    return 0;
}

int verification_sub_modify_cleanup(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    test->api->close_fd(test->params->fd);
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->delete_file(filename);
    }
    free_test_params(test->params);
    test->api->delete_file("WATZ");
    return 0;
}


/// MICROBENCHMARKS
// FILE METADATA

// CREATE FILE
int file_metadata_create_test_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->count = FILES_TO_CREATE;
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
    test->params->count = FILES_TO_CREATE;
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

// OPEN FILE
int file_metadata_open_test_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->count = FILES_TO_CREATE;
    char filename[MAX_FILENAME_SIZE];
    void* t = malloc(sizeof(int) * FILES_TO_CREATE);
    test->params->fds = (int*)t;
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->api->create_file(filename);
    }
    return 0;
}
int file_metadata_open_test_run(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        sprintf(filename, "%d", i);
        test->params->fds[i] = test->api->open_get_fd(filename);
    }
    return 0;
}

int file_metadata_open_test_cleanup(struct Test* test){
    char filename[MAX_FILENAME_SIZE];
    for(int i = 0; i < test->params->count; i++){
        test->api->close_fd(test->params->fds[i]);
        sprintf(filename, "%d", i);
        test->api->delete_file(filename);
    }
    free_test_params(test->params);
    return 0;
}

// THROUGHPUT
// SEQ READS
int throughput_seq_read_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->filename = "WATZ";
    test->params->count = FILES_TO_CREATE;
    void* t = malloc(WRITE_BYTES);
    test->params->buffer = (char*)t;
    test->api->create_file(test->params->filename);
    test->params->fd = test->api->open_get_fd(test->params->filename);
    test->api->write_at(test->params->fd, 0, WRITE_BYTES, test->params->buffer);
    return 0;
}

int throughput_seq_read_run(struct Test* test){
    for(int i = 0; i < test->params->count; i++){
        test->api->read_at(test->params->fd, 0, WRITE_BYTES, test->params->buffer);
    }
    return 0;
}

int throughput_seq_read_cleanup(struct Test* test){
    test->api->close_fd(test->params->fd);
    test->api->delete_file(test->params->filename);
    free_test_params(test->params);
    test->params = NULL;
    return 0;
}

// SEQ WRITE
int throughput_seq_write_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->filename = "WATZ";
    test->params->count = FILES_TO_CREATE;
    void* t = malloc(WRITE_BYTES);
    test->params->buffer = (char*)t;
    test->api->create_file(test->params->filename);
    test->params->fd = test->api->open_get_fd(test->params->filename);
    return 0;
}

int throughput_seq_write_run(struct Test* test){
    for(int i = 0; i < test->params->count; i++){
        test->api->write_at(test->params->fd, 0, WRITE_BYTES, test->params->buffer);
    }
    return 0;
}

int throughput_seq_write_cleanup(struct Test* test){
    test->api->close_fd(test->params->fd);
    test->api->delete_file(test->params->filename);
    free_test_params(test->params);
    return 0;
}

// RAND READ
int throughput_rand_read_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->filename = "WATZ";
    test->params->count = FILES_TO_CREATE;
    void* t = malloc(sizeof(int)*FILES_TO_CREATE);
    test->params->rands = (int*)t;
    t = malloc(sizeof(char)*WRITE_BYTES);
    test->params->buffer = (char*)t;
    for(int i = 0; i < test->params->count; i++){
        test->params->rands[i] = (random_rand() % WRITE_BYTES);
    }
    test->api->create_file(test->params->filename);
    test->params->fd = test->api->open_get_fd(test->params->filename);
    return 0;
}
int throughput_rand_read_run(struct Test* test){
    for(int i = 0; i < test->params->count; i++){
        test->api->read_at(test->params->fd, test->params->rands[i], RAND_READ_SIZE, test->params->buffer);
    }
    return 0;
}

int throughput_rand_read_cleanup(struct Test* test){
    test->api->close_fd(test->params->fd);
    test->api->delete_file(test->params->filename);
    free_test_params(test->params);
    return 0;
}

// RAND WRITE
int throughput_rand_write_prepare(struct Test* test){
    test->params = new_test_params();
    test->params->filename = "WATZ";
    test->params->count = FILES_TO_CREATE;
    void* t = malloc(sizeof(int)*FILES_TO_CREATE);
    test->params->rands = (int*)t;
    t = malloc(sizeof(char)*RAND_WRITE_SIZE);
    test->params->buffer = (char*)t;
    for(int i = 0; i < test->params->count; i++){
        test->params->rands[i] = (random_rand() % WRITE_BYTES);
    }
    test->api->create_file(test->params->filename);
    test->params->fd = test->api->open_get_fd(test->params->filename);
    return 0;
}
int throughput_rand_write_run(struct Test* test){
    for(int i = 0; i < test->params->count; i++){
        test->api->write_at(test->params->fd, test->params->rands[i], RAND_WRITE_SIZE, test->params->buffer);
    }
    return 0;
}
int throughput_rand_write_cleanup(struct Test* test){
    test->api->close_fd(test->params->fd);
    test->api->delete_file(test->params->filename);
    free_test_params(test->params);
    return 0;
}

/// MACROBENCHMARKS
// Archival Storage
int macrobenchmark_archival_prepare(struct Test* test){
    return 0;
}
int macrobenchmark_archival_run(struct Test* test){
    return 0;
}
int macrobenchmark_archival_cleanup(struct Test* test){
    return 0;
}

// Archival Storage And Query
int macrobenchmark_archival_query_prepare(struct Test* test){
    return 0;
}
int macrobenchmark_archival_query_run(struct Test* test){
    return 0;
}
int macrobenchmark_archival_query_cleanup(struct Test* test){
    return 0;
}

// Signal Processing
int macrobenchmark_signal_prepare(struct Test* test){
    return 0;
}
int macrobenchmark_signal_run(struct Test* test){
    return 0;
}
int macrobenchmark_signal_cleanup(struct Test* test){
    return 0;
}

// Network Routing
int macrobenchmark_network_prepare(struct Test* test){
    return 0;
}
int macrobenchmark_network_run(struct Test* test){
    return 0;
}
int macrobenchmark_network_cleanup(struct Test* test){
    return 0;
}

// Debugging Logs
int macrobenchmark_debugging_prepare(struct Test* test){
    return 0;
}
int macrobenchmark_debugging_run(struct Test* test){
    return 0;
}
int macrobenchmark_debugging_cleanup(struct Test* test){
    return 0;
}

// Calibration
int macrobenchmark_calibration_prepare(struct Test* test){
    return 0;
}
int macrobenchmark_calibration_run(struct Test* test){
    return 0;
}
int macrobenchmark_calibration_cleanup(struct Test* test){
    return 0;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Other Functions

The functions below are other misc functions needed by the testing framework
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// Verification Tests
struct Test* VerifyOpenUncached;
struct Test* VerifyModifyInitial;
struct Test* VerifyOpenCached;
// Microbenchmarks
struct Test* FileMetaDataCreate;
struct Test* FileMetaDataDelete;
struct Test* ThroughputSeqRead;
struct Test* FileMetaDataOpen;
struct Test* ThroughputSeqWrite;
struct Test* ThroughputRandRead;
struct Test* ThroughputRandWrite;
// Macrobenchmarks
struct Test* ArchivalStorage;
struct Test* ArchivalStorageAndQuery;
struct Test* SignalProcessing;
struct Test* NetworkRouting;
struct Test* DebuggingLogs;
struct Test* Calibration;

/*
init_test is called when the program starts.

todo: make these only be created depending on tests
being run...
*/
void init_test(){
    VerifyOpenUncached = new_test(
        "Verification Test - Open Uncached File",
        verification_open_uncached_prepare,
        verification_open_uncached_run,
        verification_open_uncached_cleanup
    );
    VerifyOpenCached = new_test(
        "Verification Test - Open Cached File",
        verification_open_cached_prepare,
        verification_open_cached_run,
        verification_open_cached_cleanup
    );
    VerifyModifyInitial = new_test(
        "Verification Test - Initial Modify",
        verification_initial_modify_prepare,
        verification_initial_modify_run,
        verification_initial_modify_cleanup
    );
    VerifyModifySub = new_test(
        "Verification Test - Subsequent Modify",
        verification_sub_modify_prepare,
        verification_sub_modify_run,
        verification_sub_modify_cleanup
    );
    FileMetaDataCreate= new_test(
        "Metadata Test - Create Files",
        file_metadata_create_test_prepare,
        file_metadata_create_test_run,
        file_metadata_create_test_cleanup
    );
    FileMetaDataDelete= new_test(
        "Metadata Test - Delete Files",
        file_metadata_delete_test_prepare,
        file_metadata_delete_test_run,
        file_metadata_delete_test_cleanup
    );
    FileMetaDataOpen= new_test(
        "Metadata Test - Open Files",
        file_metadata_open_test_prepare,
        file_metadata_open_test_run,
        file_metadata_open_test_cleanup
    );
    ThroughputSeqRead= new_test(
        "Throughput Test - Sequential Read",
        throughput_seq_read_prepare,
        throughput_seq_read_run,
        throughput_seq_read_cleanup
    );
    ThroughputSeqWrite= new_test(
        "Throughput Test - Sequential Write",
        throughput_seq_write_prepare,
        throughput_seq_write_run,
        throughput_seq_write_cleanup
    );
    ThroughputRandRead= new_test(
        "Throughput Test - Random Read",
        throughput_rand_read_prepare,
        throughput_rand_read_run,
        throughput_rand_read_cleanup
    );
    ThroughputRandWrite= new_test(
        "Throughput Test - Random Write",
        throughput_rand_write_prepare,
        throughput_rand_write_run,
        throughput_rand_write_cleanup
    );
    ArchivalStorage= new_test(
        "Macrobench - Archival Storage",
        macrobenchmark_archival_prepare,
        macrobenchmark_archival_run,
        macrobenchmark_archival_cleanup
    );
    ArchivalStorageAndQuery= new_test(
        "Macrobench - Archival Storage And Query",
        macrobenchmark_archival_query_prepare,
        macrobenchmark_archival_query_run,
        macrobenchmark_archival_query_cleanup
    );
    SignalProcessing= new_test(
        "Macrobench - Signal Processing",
        macrobenchmark_signal_prepare,
        macrobenchmark_signal_run,
        macrobenchmark_signal_cleanup
    );
    NetworkRouting= new_test(
        "Macrobench - Network Routing",
        macrobenchmark_network_prepare,
        macrobenchmark_network_run,
        macrobenchmark_network_cleanup
    );
    DebuggingLogs= new_test(
        "Macrobench - Debugging Logs",
        macrobenchmark_debugging_prepare,
        macrobenchmark_debugging_run,
        macrobenchmark_debugging_cleanup
    );
    Calibration= new_test(
        "Macrobench - Calibration",
        macrobenchmark_calibration_prepare,
        macrobenchmark_calibration_run,
        macrobenchmark_calibration_cleanup
    );
    return;
}

/*
cleanup_test is called before the program exits
*/
void cleanup_test(){
    free_test(VerifyOpenUncached);
    free_test(VerifyModifyInitial);
    free_test(VerifyModifySub);
    free_test(VerifyOpenCached);
    free_test(FileMetaDataCreate);
    free_test(FileMetaDataDelete);
    free_test(FileMetaDataOpen);
    free_test(ThroughputSeqRead);
    free_test(ThroughputSeqWrite);
    free_test(ThroughputRandRead);
    free_test(ThroughputRandWrite);
    free_test(ArchivalStorage);
    free_test(ArchivalStorageAndQuery);
    free_test(SignalProcessing);
    free_test(NetworkRouting);
    free_test(DebuggingLogs);
    free_test(Calibration);
    return;
}
