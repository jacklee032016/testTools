Timestamp of ethernet driver
##########################################
June 2, 2019

Run program on sama5d2 board;

Tests
====================

List all timestamp options support by OS:

sudo ethtool -T eth0


timestamping eth0 SOF_TIMESTAMPING_RX_HARDWARE
SIOCSHWTSTAMP: Operation not supported

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
		