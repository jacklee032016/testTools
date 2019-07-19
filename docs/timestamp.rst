Timestamp of ethernet driver
##########################################
June 2, 2019

Timestamp type in kernel
===============================
Timestamp for packets in network stack
#. SO_TIMESTAMP
    in usec resolution;
	for receiving datagram packets;
	in recvmsg() control message;
# SO_TIMESTAMPNS
    in nsec resolution;
* SO_TIMESTAMPING
	* Generating:
        hardware-transmit     (SOF_TIMESTAMPING_TX_HARDWARE)
        software-transmit     (SOF_TIMESTAMPING_TX_SOFTWARE)
        hardware-receive      (SOF_TIMESTAMPING_RX_HARDWARE)
        software-receive      (SOF_TIMESTAMPING_RX_SOFTWARE)
	* Reporting: in control message;
        software-system-clock (SOF_TIMESTAMPING_SOFTWARE)
        hardware-raw-clock    (SOF_TIMESTAMPING_RAW_HARDWARE)
	* Options: not used now


refer to documents/networking/timestamping.txt and include/uapi/linux/net_tstamp.h;

Code: ethtool.c hwstamp_ctl.c in linuxptp


Run program on sama5d2 board;

Tests
====================

List all timestamp options support by OS:

sudo ethtool -T eth0

::

    ethtool -T eth3
    Time stamping parameters for eth3:
    Capabilities:
	
        hardware-transmit     (SOF_TIMESTAMPING_TX_HARDWARE)
        software-transmit     (SOF_TIMESTAMPING_TX_SOFTWARE)
        hardware-receive      (SOF_TIMESTAMPING_RX_HARDWARE)
        software-receive      (SOF_TIMESTAMPING_RX_SOFTWARE)
        software-system-clock (SOF_TIMESTAMPING_SOFTWARE)
        hardware-raw-clock    (SOF_TIMESTAMPING_RAW_HARDWARE)
    PTP Hardware Clock: 0
    Hardware Transmit Timestamp Modes:
        off                   (HWTSTAMP_TX_OFF)
        on                    (HWTSTAMP_TX_ON)
    Hardware Receive Filter Modes:
        none                  (HWTSTAMP_FILTER_NONE)
        all                   (HWTSTAMP_FILTER_ALL)



    sudo ethtool -T eth0
    [sudo] password for lzj: 
    Time stamping parameters for eth0:
    Capabilities:
	    software-transmit     (SOF_TIMESTAMPING_TX_SOFTWARE)
	    software-receive      (SOF_TIMESTAMPING_RX_SOFTWARE)
	    software-system-clock (SOF_TIMESTAMPING_SOFTWARE)
    PTP Hardware Clock: none
    Hardware Transmit Timestamp Modes: none
    Hardware Receive Filter Modes: none



Resolve the problem
-------------------------

::

	./hwstampConfig eth0 OFF NONE
	ioctl: Operation not supported

	SIOCGHWTSTAMP/SIOCSHWTSTAMP

Add and init ptp module in ethernet driver:

::
	
	tx_type is any of (case-insensitive):
		OFF
		ON
		ONESTEP_SYNC
	rx_filter is any of (case-insensitive):
		NONE
		ALL
		SOME
		PTP_V1_L4_EVENT
		PTP_V1_L4_SYNC
		PTP_V1_L4_DELAY_REQ
		PTP_V2_L4_EVENT
		PTP_V2_L4_SYNC
		PTP_V2_L4_DELAY_REQ
		PTP_V2_L2_EVENT
		PTP_V2_L2_SYNC
		PTP_V2_L2_DELAY_REQ
		PTP_V2_EVENT
		PTP_V2_SYNC
		PTP_V2_DELAY_REQ
		