#
# $Author$
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
	.


CFLAGS += -Wall -DVER=2.0 -D_GNU_SOURCE -DHAVE_CLOCK_ADJTIME -DHAVE_POSIX_SPAWN -DHAVE_ONESTEP_SYNC  

SRC:= \
			sysoff.c \
			pmc_common.c \
			version.c \
			util.c \
			unicast_service.c \
			unicast_fsm.c \
			unicast_client.c \
			uds.c \
			udp6.c \
			udp.c \
			tsproc.c \
			transport.c \
			tlv.c \
			telecom.c \
			tc.c \
			stats.c \
			sk.c \
			servo.c \
			rtnl.c \
			raw.c \
			p2p_tc.c \
			print.c \
			pqueue.c \
			port_signaling.c \
			port.c \
			pi.c \
			phc.c \
			nullf.c \
			ntpshm.c \
			msg.c \
			mmedian.c \
			mave.c \
			linreg.c \
			hash.c \
			fsm.c \
			filter.c \
			fault.c \
			e2e_tc.c \
			config.c \
			clockcheck.c \
			clockadj.c \
			clock.c \
			bmc.c \

	
NOT_USED += \
			ptp4l.c \
			hwstamp_ctl.c \
			pmc.c \
			timemaster.c \
			nsm.c \
			phc2sys.c \
			phc_ctl.c \


LOCAL_ALL := \
		ptp4l \
		hwstamp_ctl \
		nsm \
		phc2sys \
		phc_ctl \
		pmc \
		timemaster \
		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 

ifeq ($(ARCH),arm)
LDFLAGS += $(HI_LIB_PATHS) 
else
LDFLAGS += -lpthread -ldl
endif

LDFLAGS += -lrt -lm


ptp4l:$(OBJ_DIR)/ptp4l.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/bin  

hwstamp_ctl:$(OBJ_DIR)/hwstamp_ctl.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

nsm:$(OBJ_DIR)/nsm.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

phc2sys:$(OBJ_DIR)/phc2sys.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

phc_ctl:$(OBJ_DIR)/phc_ctl.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

pmc:$(OBJ_DIR)/pmc.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

timemaster:$(OBJ_DIR)/timemaster.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/bin  

