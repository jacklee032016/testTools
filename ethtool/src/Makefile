#
# for ethtool-5.1
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
	.


CFLAGS += -DHAVE_CONFIG_H -I. 

SRC:= \
			ethtool.c \
			rxclass.c \
			amd8111e.c \
			de2104x.c \
			dsa.c \
			e100.c \
			e1000.c \
			et131x.c \
			igb.c \
			fec.c \
			fec_8xx.c \
			ibm_emac.c \
			ixgb.c \
			ixgbe.c \
			natsemi.c \
			pcnet32.c \
			realtek.c \
			tg3.c \
			marvell.c \
			vioc.c \
			smsc911x.c \
			at76c50x-usb.c \
			sfc.c \
			stmmac.c \
			sff-common.c \
			sfpid.c \
			sfpdiag.c \
			ixgbevf.c \
			tse.c \
			vmxnet3.c \
			qsfp.c \
			fjes.c \
			lan78xx.c \
			
	

LOCAL_ALL := \
		ethtool \
		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 


LDFLAGS += -lm
# -lrt

$(LOCAL_ALL):$(OBJS)
		$(CC) -s -o $@ $(OBJS) -L$(ROOT_DIR)/Linux.bin.$(ARCH)/lib $(LDFLAGS) $(LIBS) 
		$(MOVE) $@ $(BIN_DIR)/sbin
