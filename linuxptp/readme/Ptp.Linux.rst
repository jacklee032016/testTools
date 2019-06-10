PTP Linux
#############################
06.09, 2019

PTP Clock interface to kernel
====================================
include/uapi/linux/ptp_clock.h

clockid_t and clock_gettime
---------------------------------
linux/time.h defines symbal name of clockid_t, such as CLOCK_REALTIME;

clock_it and int clock_gettime(clockid_t clock_id, struct timespec *tp);


PTP clock can be used as one kind of clockid_t;
