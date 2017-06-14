DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
CONTIKI_PROJECT = watzbench
PROJECT_SOURCEFILES = test.c common.c api.c
CFLAGS += -std=gnu99
APPS+=powertrace

all: $(CONTIKI_PROJECT)

CONTIKI_WITH_IPV6 = 1

CONTIKI = /home/user/contiki-2.7
include $(CONTIKI)/Makefile.include
