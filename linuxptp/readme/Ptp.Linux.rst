PTP Linux
#############################
06.09, 2019


Programs
====================================

nsm
----------------------------------
NetSync Monitor, definied by meinberge;

NSM allows a node with a local time reference (like a
GM using GPS) to measure the offset of a given clock.  In a nutshell,
NSM works by having the node to be measured act like a unicast master
clock on demand.

1. The monitoring node sends a unicast delay request with a special TLV attached.

2. The node to be measured replies with a unicast delay response, 
   followed by unicast Sync and FollowUp messages.

3. Once the monitoring node receives the three responses, it
   calculates the measured node's offset.


One the nice features of NSM is that, when used from the GM, it allows
one to empirically determine the end to end asymmetry in the network.
The details of the protocol are described in a brief PDF published by
Meinberg.  If anyone would like a copy, please contact me off list.


pmc - PTP management client
----------------------------------


----------------------------------


PTP Clock interface to kernel
====================================
include/uapi/linux/ptp_clock.h

clockid_t and clock_gettime
---------------------------------
linux/time.h defines symbal name of clockid_t, such as CLOCK_REALTIME;

clock_it and int clock_gettime(clockid_t clock_id, struct timespec *tp);


PTP clock can be used as one kind of clockid_t;
