Clock and Timer of Linux 
##########################################
July 15, 2019


Clocks
====================
System created clocks, which can be accessed and avaiable by one process or cross multiple processes;

Call the system calls directly from -lrt with following APIs:

Typical clocks:CLOCK_REALTIME, CLOCK_MONOTONIC, CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID 

::

    #include <time.h>

    int clock_getres(clockid_t clk_id, struct timespec *res);
    int clock_gettime(clockid_t clk_id, struct timespec *tp);
    int clock_settime(clockid_t clk_id, const struct timespec *tp); 


    struct timespec {
        time_t   tv_sec;        /* seconds */
        long     tv_nsec;       /* nanoseconds */
    };


PTP Clock
-------------------------
PTP device /dev/ptp0, open and transformed into clockid_t;

::

    #include <sys/syscall.h>
    static int clock_adjtime(clockid_t id, struct timex *tx)
    {
	    return syscall(__NR_clock_adjtime, id, tx);
    }

    static clockid_t get_clockid(int fd)
    {
    #define CLOCKFD 3
    #define FD_TO_CLOCKID(fd)	((~(clockid_t) (fd) << 3) | CLOCKFD)

	    return FD_TO_CLOCKID(fd);
    }


So PTP device can be acccessed as ptp device and clock device;

tune kernel clock
-------------------------
       adjtimex, ntp_adjtime - tune kernel clock

::

    #include <sys/timex.h>

    int adjtimex(struct timex *buf);

    int ntp_adjtime(struct timex *buf);



Timers
====================

Standard APIs from lib C
-------------------------
Creation
+++++++++++++++++++++++++++++++
Create a timer from clock and register a signal handle with this timer:

::

    #include <signal.h>
    #include <time.h>

    int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid);

Creation
+++++++++++++++++++++++++++++++
Create a timer from clock and register a signal handle with this timer:

::

    int timer_settime(timer_t timerid, int flags,  
                         const struct itimerspec *new_value,
                         struct itimerspec *old_value);
    int timer_gettime(timer_t timerid, struct itimerspec *curr_value);

    
	struct itimerspec {
               struct timespec it_interval;  /* Timer interval */
               struct timespec it_value;     /* Initial expiration */
           };


Timer from system calls
-------------------------
Timer with indiffrent file descriptor and can be polled

::

    int timerfd_create(int clockid, int flags)
    {
	     return syscall(__NR_timerfd_create, clockid, flags);
    }

    int timerfd_settime(int ufc, int flags, const struct itimerspec *utmr, struct itimerspec *otmr)
    {
	    return syscall(__NR_timerfd_settime, ufc, flags, utmr, otmr);
    }

    int timerfd_gettime(int ufc, struct itimerspec *otmr)
    {
	    return syscall(__NR_timerfd_gettime, ufc, otmr);
    }


	