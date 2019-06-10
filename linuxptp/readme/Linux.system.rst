Linux interfaces
####################################
06.09, 2019


System call
=========================
defined in <sys/syscall.h>, so it is a C interface from C library which calls the asmblly interface of system call;
eg, this header file is not come from OS directly;

In bits/syscall.h, that __NR_XXX is come from OS directly, eg. asm/unistd.h;

#define SYS_gettid __NR_gettid
