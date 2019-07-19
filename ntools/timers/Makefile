#
# $Author$
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
	.


# CFLAGS += -Wall -DVER=2.0 -D_GNU_SOURCE -DHAVE_CLOCK_ADJTIME -DHAVE_POSIX_SPAWN -DHAVE_ONESTEP_SYNC -I../include

SRC:= \

	

LOCAL_ALL := \
		testSysCalls	\
		testTimerFd \
		testTimerFd2 \
		timers
		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 


LDFLAGS += -lrt


testSysCalls:$(OBJ_DIR)/testSysCalls.o 
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/bin  

testTimerFd:$(OBJ_DIR)/testTimerFd.o
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

testTimerFd2:$(OBJ_DIR)/testTimerFd2.o
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

timers:$(OBJ_DIR)/timers.o
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

