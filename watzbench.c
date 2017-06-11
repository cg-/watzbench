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

  int err = CFS->create_file("test1");
  check(err, "creating file", TRUE);

  err = CFS->delete_file("test1");
  check(err, "delete file", TRUE);

  cleanup();
  PROCESS_END();
}