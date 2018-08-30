#
# $Author$
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
	.


CFLAGS += -Wall -DVER=2.0 -D_GNU_SOURCE -DHAVE_CLOCK_ADJTIME -DHAVE_POSIX_SPAWN -DHAVE_ONESTEP_SYNC  

SRC:= \
			tgetopt.c \
			udp.c \

	

LOCAL_ALL := \
		mpong \
		msend \
		mdump \
		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 

ifeq ($(ARCH),arm)
LDFLAGS += $(HI_LIB_PATHS) 
else
LDFLAGS += -lpthread -ldl
endif

LDFLAGS += -lrt -lm


mdump:$(OBJ_DIR)/mdump.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/bin  

mpong:$(OBJ_DIR)/mpong.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

msend:$(OBJ_DIR)/msend.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

