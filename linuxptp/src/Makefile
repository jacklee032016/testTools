#
# $Id$
#

include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
					.


C_FLAGS += $(SHARED_CFLAGS) -Wall -DVER=2.0 -D_GNU_SOURCE -DHAVE_CLOCK_ADJTIME -DHAVE_POSIX_SPAWN -DHAVE_ONESTEP_SYNC -I../include 


################################################
####   source file 
################################################
PORTABL_SRC += \

SRC += \
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
		
 
TARGET_LIB:=libPtp4l

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post

