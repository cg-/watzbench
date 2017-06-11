CONTIKI_PROJECT = watzbench
PROJECT_SOURCEFILES = api.c test.c common.c
CFLAGS += -std=gnu99

all: $(CONTIKI_PROJECT)

#UIP_CONF_IPV6=1

CONTIKI = /home/user/contiki
include $(CONTIKI)/Makefile.include
