#
# $Author$
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
	.


CFLAGS += -Wall -DVER=2.0 -D_GNU_SOURCE -DHAVE_CLOCK_ADJTIME -DHAVE_POSIX_SPAWN -DHAVE_ONESTEP_SYNC  

SRC:= \

	

LOCAL_ALL := \
		timestamping \
		hwstampConfig \

ifeq ($(ARCH),arm)
LOCAL_ALL += \
		txtimestamp 

endif		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 

ifeq ($(ARCH),arm)
LDFLAGS += $(HI_LIB_PATHS) 
else
LDFLAGS += -lpthread -ldl
endif

# LDFLAGS += -lrt -lm


txtimestamp:$(OBJ_DIR)/txtimestamp.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/bin  

timestamping:$(OBJ_DIR)/timestamping.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

hwstampConfig:$(OBJ_DIR)/hwstampConfig.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

