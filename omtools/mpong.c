/* mpong.c */
/*   Program to do ping-pong round-trip latency measurements using
 * multicast.
 * See https://community.informatica.com/solutions/1470 for more info.
 *
 * Author: 29West/Informatica (with small parts of the code borrowed
 * from J.P.Knight@lut.ac.uk's "mdump" program)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted without restriction.
 *
 THE SOFTWARE IS PROVIDED "AS IS" AND INFORMATICA DISCLAIMS ALL WARRANTIES
 EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY IMPLIED WARRANTIES OF
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR
 PURPOSE.  INFORMATICA DOES NOT WARRANT THAT USE OF THE SOFTWARE WILL BE
 UNINTERRUPTED OR ERROR-FREE.  INFORMATICA SHALL NOT, UNDER ANY CIRCUMSTANCES,
 BE LIABLE TO LICENSEE FOR LOST PROFITS, CONSEQUENTIAL, INCIDENTAL, SPECIAL OR
 INDIRECT DAMAGES ARISING OUT OF OR RELATED TO THIS AGREEMENT OR THE
 TRANSACTIONS CONTEMPLATED HEREUNDER, EVEN IF INFORMATICA HAS BEEN APPRISED OF
 THE LIKELIHOOD OF SUCH DAMAGES.
 */

/*
 * modified by Aviad Rozenhek [aviadr1@gmail.com] for open-mtools
 */

#include "mtools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* Many of the following definitions are intended to make it easier to write
 * portable code between windows and unix. */

#define EXIT(x) do { fprintf(stdout, "Exit, file: '%s', line: %d\n", __FILE__, __LINE__);  exit(x);  } while (0)

typedef struct mpong_options {
    /* program name (from argv[0] */
    char *prog_name;

    /* program options */
    int o_initiator;
    FILE *o_output;
    int o_rcvbuf_size;
    int o_Sndbuf_size;
    int o_samples;
    int o_verbose;

    /* program positional parameters */
    unsigned long int groupaddr;
    unsigned short int groupport;
    unsigned char ttlvar;
    char *bind_if;


    struct timeval *start_tvs;
    struct timeval *end_tvs;
} mpong_options;


static const char usage_str[] = "[-h] [-i] [-o ofile] [-r rcvbuf_size] [-S Sndbuf_size] [-s samples] [-v] group port [ttl] [interface]";

void usage(mpong_options* opts, char *msg)
{
	if (msg != NULL)
		fprintf(stderr, "\n%s\n\n", msg);
	fprintf(stderr, "Usage: %s %s\n\n"
			"(use -h for detailed help)\n",
			opts->prog_name, usage_str);
}  /* usage */


void help(mpong_options* opts, char *msg)
{
	if (msg != NULL)
		fprintf(stderr, "\n%s\n\n", msg);
	fprintf(stderr, "Usage: %s %s\n", opts->prog_name, usage_str);
	fprintf(stderr, "Where:\n"
			"  -h : help\n"
			"  -i : initiator (sends first packet) [reflector]\n"
			"  -o ofile : print results to file (in addition to stdout)\n"
			"  -r rcvbuf_size : size (bytes) of UDP receive buffer (SO_RCVBUF) [4194304]\n"
			"                   (use 0 for system default buff size)\n"
			"  -S Sndbuf_size : size (bytes) of UDP send buffer (SO_SNDBUF) [65536]\n"
			"                   (use 0 for system default buff size)\n"
			"  -s samples : number of cycles to measure [65536]\n"
			"  -v : verbose (print each RTT sample)\n"
			"\n"
			"  group : multicast address to send on (use '0.0.0.0' for unicast)\n"
			"  port : destination port\n"
			"  interface : optional IP addr of local interface (for multi-homed hosts) [INADDR_ANY]\n"
			"\n"
			"Note: initiator sends on supplied port + 1, reflector replies on supplied port\n"
	);
}  /* help */


/* After doing arithmetic on tv variables, you can end up with strange values.
 * (Code borrowed from ACE) */
void normalize_tv(struct timeval *tv)
{
	if (tv->tv_usec >= 1000000) {
		do { tv->tv_sec++; tv->tv_usec -= 1000000; } while (tv->tv_usec >= 1000000);
	} else if (tv->tv_usec <= -1000000) {
		do { tv->tv_sec--; tv->tv_usec += 1000000; } while (tv->tv_usec <= -1000000);
	}
	if (tv->tv_sec >= 1 && tv->tv_usec < 0) {
		tv->tv_sec--; tv->tv_usec += 1000000;
	} else if (tv->tv_sec < 0 && tv->tv_usec > 0) {
		tv->tv_sec++; tv->tv_usec -= 1000000;
	}
}  /* normalize_tv */


/* Utility function to return current time of day */
void current_tv(struct timeval *tv)
{
#if defined(_WIN32)
	LARGE_INTEGER ticks;
	static LARGE_INTEGER freq;
	static int first = 1;

	if (first) {
		QueryPerformanceFrequency(&freq);
		first = 0;
	}
	QueryPerformanceCounter(&ticks);
	tv->tv_sec = 0;
	tv->tv_usec = (long)(1000000 * ticks.QuadPart / freq.QuadPart);
	normalize_tv(tv);
#else 
	gettimeofday(tv,NULL);
#endif /* _WIN32 */
}  /* current_tv */


int main(int argc, char **argv)
{
	int opt;
	int num_parms;
	char *buff;
	SOCKET sock;
	socklen_t fromlen = sizeof(struct sockaddr_in);
	int default_rcvbuf_sz, cur_size, sz;
	int num_rcvd;
	struct sockaddr_in in_sa;
	struct sockaddr_in out_sa;
	struct sockaddr_in src;
	unsigned int wttl;
	struct ip_mreq imr;
	struct timeval first_tv;
	struct timeval start_tv;
	struct timeval end_tv;
	struct timeval delta_tv;
	struct timeval sum_tv;
	struct timeval max_tv;
	struct timeval min_tv;
	char timestr1[65], timestr2[65];
	float avg;
	float cur;
	float std;
#if defined(_WIN32)
	unsigned long int iface_in;
#else
	struct in_addr iface_in;
#endif /* _WIN32 */
    mpong_options opts; 
    memset(&opts, sizeof(opts), 0);
	opts.prog_name = argv[0];

	buff = malloc(65536 + 1);  /* one extra for trailing null (if needed) */
	if (buff == NULL) { fprintf(stderr, "malloc failed\n"); EXIT(1); }

#if defined(_WIN32)
	{
		WSADATA wsadata;  int wsstatus;
		if ((wsstatus = WSAStartup(MAKEWORD(2,2), &wsadata)) != 0) {
			fprintf(stderr,"%s: WSA startup error - %d\n", argv[0], wsstatus);
			EXIT(1);
		}
	}
#else
	signal(SIGPIPE, SIG_IGN);
#endif /* _WIN32 */

	/* get system default value for socket buffer size */
	if((sock = socket(PF_INET,SOCK_DGRAM,0)) == INVALID_SOCKET) {
		fprintf(stderr, "ERROR: ");  PERROR((&opts), "socket");
		EXIT(1);
	}
	sz = sizeof(default_rcvbuf_sz);
	if (getsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char *)&default_rcvbuf_sz,
			(socklen_t *)&sz) == SOCKET_ERROR) {
		fprintf(stderr, "ERROR: ");  PERROR((&opts), "getsockopt - SO_RCVBUF");
		EXIT(1);
	}
	CLOSESOCKET(sock);

	/* default values for options */
	opts.o_initiator = 0;
	opts.o_output = NULL;
	opts.o_rcvbuf_size = 0x100000;  /* 1MB */
	opts.o_Sndbuf_size = 65536;
	opts.o_samples = 65536;
	opts.o_verbose = 0;

	/* default values for optional positional params */
	opts.ttlvar = 2;
	opts.bind_if = NULL;

	while ((opt = tgetopt(argc, argv, "hio:r:S:s:v")) != EOF) {
		switch (opt) {
		  case 'h':
			help(&opts, NULL);  exit(0);
			break;
		  case 'i':
			opts.o_initiator = 1;
			break;
		  case 'o':
			if (strlen(toptarg) > 1000) {
				fprintf(stderr, "ERROR: file name too long (%s)\n", toptarg);
				EXIT(1);
			}
			opts.o_output = fopen(toptarg, "w");
			if (opts.o_output == NULL) {
				fprintf(stderr, "ERROR: ");  PERROR((&opts), "fopen");
				EXIT(1);
			}
			break;
		  case 'r':
			opts.o_rcvbuf_size = atoi(toptarg);
			if (opts.o_rcvbuf_size == 0)
				opts.o_rcvbuf_size = default_rcvbuf_sz;
			break;
		  case 'S':
			opts.o_Sndbuf_size = atoi(toptarg);
			break;
		  case 's':
			opts.o_samples = atoi(toptarg);
			break;
		  case 'v':
			opts.o_verbose = 1;
			break;
		  default:
			usage(&opts, "unrecognized option");
			EXIT(1);
			break;
		}  /* switch */
	}  /* while opt */

	num_parms = argc - toptind;

	/* handle positional parameters */
	if (num_parms == 2) {
		opts.groupaddr = inet_addr(argv[toptind]);
		opts.groupport = (unsigned short)atoi(argv[toptind+1]);
	} else if (num_parms == 3) {
		char c;  int i;
		opts.groupaddr = inet_addr(argv[toptind]);
		opts.groupport = (unsigned short)atoi(argv[toptind+1]);
		for (i = 0; (c = *(argv[toptind+2] + i)) != '\0'; ++i) {
			if (c < '0' || c > '9') {
				fprintf(stderr, "ERROR: third positional argument '%s' has non-numeric.  Should be TTL.\n", argv[toptind+2]);
				EXIT(1);
			}
		}
		opts.ttlvar = (unsigned char)atoi(argv[toptind+2]);
	} else if (num_parms == 4) {
		opts.groupaddr = inet_addr(argv[toptind]);
		opts.groupport = (unsigned short)atoi(argv[toptind+1]);
		opts.ttlvar = (unsigned char)atoi(argv[toptind+2]);
		opts.bind_if = argv[toptind+3];
	} else {
		usage(&opts, "need 2-4 positional parameters");
		EXIT(1);
	}

	if((sock = socket(PF_INET,SOCK_DGRAM,0)) == INVALID_SOCKET) {
		fprintf(stderr, "ERROR: ");  PERROR((&opts), "socket");
		EXIT(1);
	}

	if(setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(const char *)&opts.o_rcvbuf_size,
			sizeof(opts.o_rcvbuf_size)) == SOCKET_ERROR) {
		printf("WARNING: setsockopt - SO_RCVBUF\n"); fflush(stdout);
		if (opts.o_output) { fprintf(opts.o_output, "WARNING: "); PERROR((&opts), "setsockopt - SO_RCVBUF"); fflush(opts.o_output); }
	}
	sz = sizeof(cur_size);
	if (getsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char *)&cur_size,
			(socklen_t *)&sz) == SOCKET_ERROR) {
		fprintf(stderr, "ERROR: ");  PERROR((&opts), "getsockopt - SO_RCVBUF");
		EXIT(1);
	}
	if (cur_size < opts.o_rcvbuf_size) {
		printf("WARNING: tried to set SO_RCVBUF to %d, only got %d\n", opts.o_rcvbuf_size, cur_size); fflush(stdout);
		if (opts.o_output) { fprintf(opts.o_output, "WARNING: tried to set SO_RCVBUF to %d, only got %d\n", opts.o_rcvbuf_size, cur_size); fflush(opts.o_output); }
	}

	if(setsockopt(sock,SOL_SOCKET,SO_SNDBUF,(const char *)&opts.o_Sndbuf_size,
			sizeof(opts.o_Sndbuf_size)) == SOCKET_ERROR) {
		printf("WARNING: setsockopt - SO_SNDBUF\n"); fflush(stdout);
		if (opts.o_output) { fprintf(opts.o_output, "WARNING: "); PERROR((&opts), "setsockopt - SO_SNDBUF"); fflush(opts.o_output); }
	}
	sz = sizeof(cur_size);
	if (getsockopt(sock,SOL_SOCKET,SO_SNDBUF,(char *)&cur_size,
			(socklen_t *)&sz) == SOCKET_ERROR) {
		fprintf(stderr, "ERROR: ");  PERROR((&opts), "getsockopt - SO_SNDBUF");
		EXIT(1);
	}
	if (cur_size < opts.o_Sndbuf_size) {
		printf("WARNING: tried to set SO_SNDBUF to %d, only got %d\n", opts.o_Sndbuf_size, cur_size); fflush(stdout);
		if (opts.o_output) { fprintf(opts.o_output, "WARNING: tried to set SO_SNDBUF to %d, only got %d\n", opts.o_Sndbuf_size, cur_size); fflush(opts.o_output); }
	}

/* set TTL */
#if defined(_WIN32)
	wttl = opts.ttlvar;
	if (setsockopt(sock,IPPROTO_IP,IP_MULTICAST_TTL,(char *)&wttl,
				sizeof(wttl)) == SOCKET_ERROR) {
		fprintf(stderr, "ERROR: ");  PERROR((&opts), "setsockopt - TTL");
		EXIT(1);
	}
#else
	if (setsockopt(sock,IPPROTO_IP,IP_MULTICAST_TTL,(char *)&opts.ttlvar,
				sizeof(opts.ttlvar)) == SOCKET_ERROR) {
		fprintf(stderr, "ERROR: ");  PERROR((&opts), "setsockopt - TTL");
		EXIT(1);
	}
#endif /* _WIN32 */

	memset((char *)&imr,0,sizeof(imr));
	imr.imr_multiaddr.s_addr = opts.groupaddr;
	if (opts.bind_if != NULL) {
		imr.imr_interface.s_addr = inet_addr(opts.bind_if);  /* to add membership for receiving */
#if !defined(_WIN32)
		memset((char *)&iface_in,0,sizeof(iface_in));
		iface_in.s_addr = inet_addr(opts.bind_if);
#else
		iface_in = inet_addr(opts.bind_if);
#endif
		if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (const char*)&iface_in, sizeof(iface_in)) == SOCKET_ERROR) {
			fprintf(stderr, "ERROR: ");  PERROR((&opts), "setsockopt - IP_MULTICAST_IF");
			EXIT(1);
		}
	} else {
		imr.imr_interface.s_addr = htonl(INADDR_ANY);
	}

	opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == SOCKET_ERROR) {
		fprintf(stderr, "ERROR: ");  PERROR((&opts), "setsockopt SO_REUSEADDR");
		EXIT(1);
	}

	memset((char *)&in_sa,0,sizeof(in_sa));
	in_sa.sin_family = AF_INET;
	in_sa.sin_addr.s_addr = opts.groupaddr;
	memcpy((char *)&out_sa, (char *)&in_sa, sizeof(out_sa));
	if (opts.o_initiator) {
		in_sa.sin_port = htons(opts.groupport);
		out_sa.sin_port = htons(opts.groupport+1);
	} else {
		in_sa.sin_port = htons(opts.groupport+1);
		out_sa.sin_port = htons(opts.groupport);
	}
	if (bind(sock,(struct sockaddr *)&in_sa,sizeof(in_sa)) == SOCKET_ERROR) {
		/* So OSes don't want you to bind to the m/c group. */
		in_sa.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(sock,(struct sockaddr *)&in_sa, sizeof(in_sa)) == SOCKET_ERROR) {
			fprintf(stderr, "ERROR: ");  PERROR((&opts), "bind");
			EXIT(1);
		}
	}

	if (setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,
					(char *)&imr,sizeof(struct ip_mreq)) == SOCKET_ERROR ) {
		fprintf(stderr, "ERROR: ");  PERROR((&opts), "setsockopt - IP_ADD_MEMBERSHIP");
		EXIT(1);
	}

	SLEEP_SEC(1);  /* allow multicast join to complete */

	if (opts.o_initiator) {
		opts.start_tvs = (struct timeval *)malloc(opts.o_samples * sizeof(struct timeval));
		opts.end_tvs = (struct timeval *)malloc(opts.o_samples * sizeof(struct timeval));
		memset((char *)opts.start_tvs, 0, opts.o_samples * sizeof(struct timeval));
		memset((char *)opts.end_tvs, 0, opts.o_samples * sizeof(struct timeval));

		/* The -20 allows 20 cycles to happen without measurements.  This takes care of startup costs. */
		for (num_rcvd = -20; num_rcvd < opts.o_samples; ++num_rcvd) {
			current_tv(&start_tv);
			cur_size = sendto(sock, (char *)&start_tv, sizeof(struct timeval),
						0, (struct sockaddr *)&out_sa, sizeof(out_sa));
			if (cur_size == SOCKET_ERROR) { fprintf(stderr, "ERROR: ");  PERROR((&opts), "send"); EXIT(1); }

			cur_size = recvfrom(sock, buff, 65536, 0, (struct sockaddr *)&src, &fromlen);
			current_tv(&end_tv);
			if (cur_size == SOCKET_ERROR) { fprintf(stderr, "ERROR: ");  PERROR((&opts), "recv"); EXIT(1); }

			/* start and end timestamps taken, this part of the loop is non-time-critical */

			if (num_rcvd >= 0) {  /* check returned time */
				if (num_rcvd == 0) first_tv = start_tv;
				opts.start_tvs[num_rcvd] = start_tv;
				opts.end_tvs[num_rcvd] = end_tv;
				/* sanity check (make sure payload contains start_tv) */
				if (cur_size != sizeof(struct timeval)) { fprintf(stderr, "ERROR: recvfrom rtn val %d != sizeof struct timeval %d\n", cur_size, sizeof(struct timeval)); EXIT(1); }
				if (memcmp(buff, (char *)&start_tv, sizeof(struct timeval)) != 0) { fprintf(stderr, "ERROR: recvfrom buff != start_tv\n"); EXIT(1); }
			}
		}  /* for num_rcvd */

		/* Done with active ping-pong phase; calculate results */

		/* Calculate sum, min, max */
		sum_tv.tv_sec = 0; sum_tv.tv_usec = 0;

		if (opts.o_verbose) {
			printf("timestamp RTT (in microseconds):\n"); fflush(stdout);
			if (opts.o_output) { fprintf(opts.o_output, "RTT samples:\n"); fflush(opts.o_output); }
		}
		for (num_rcvd = 0; num_rcvd < opts.o_samples; ++num_rcvd) {
			start_tv = opts.start_tvs[num_rcvd];
			end_tv = opts.end_tvs[num_rcvd];
			/* calc rtt */
			delta_tv.tv_sec = end_tv.tv_sec - start_tv.tv_sec;
			delta_tv.tv_usec = end_tv.tv_usec - start_tv.tv_usec;
			normalize_tv(&delta_tv);
			/* normalize timestamps to the start time of the test */
			start_tv.tv_sec = start_tv.tv_sec - first_tv.tv_sec;
			start_tv.tv_usec = start_tv.tv_usec - first_tv.tv_usec;
			normalize_tv(&start_tv);

			if (opts.o_verbose) {
				sprintf(timestr1, "%d.%06d", start_tv.tv_sec, start_tv.tv_usec);
				if (delta_tv.tv_sec != 0) sprintf(timestr2, "%d%06d", delta_tv.tv_sec, delta_tv.tv_usec);
				else sprintf(timestr2, "%d", delta_tv.tv_usec);
				printf("%s %s\n", timestr1, timestr2); fflush(stdout);
				if (opts.o_output) { fprintf(opts.o_output, "%s %s\n", timestr1, timestr2); fflush(opts.o_output); }
			}

			sum_tv.tv_sec += delta_tv.tv_sec;  sum_tv.tv_usec += delta_tv.tv_usec;
			normalize_tv(&sum_tv);

			if (num_rcvd == 0 || delta_tv.tv_sec < min_tv.tv_sec ||
					(delta_tv.tv_sec == min_tv.tv_sec && delta_tv.tv_usec < min_tv.tv_usec)) {
				min_tv = delta_tv;
			}
			if (num_rcvd == 0 || delta_tv.tv_sec > max_tv.tv_sec ||
					(delta_tv.tv_sec == max_tv.tv_sec && delta_tv.tv_usec > max_tv.tv_usec)) {
				max_tv = delta_tv;
			}
		}

		/* Calc average and standard deviation in microseconds */
		avg = (float)sum_tv.tv_sec;  avg *= 1000000.0;  avg += (float)sum_tv.tv_usec;
		avg = avg / (float)opts.o_samples;
		std = 0.0;
		for (num_rcvd = 0; num_rcvd < opts.o_samples; ++num_rcvd) {
			start_tv = opts.start_tvs[num_rcvd];
			end_tv = opts.end_tvs[num_rcvd];
			/* calc rtt */
			delta_tv.tv_sec = end_tv.tv_sec - start_tv.tv_sec;
			delta_tv.tv_usec = end_tv.tv_usec - start_tv.tv_usec;
			normalize_tv(&delta_tv);
			cur = (float)delta_tv.tv_sec;  cur *= 1000000.0f;  cur += (float)delta_tv.tv_usec;
			std += powf((cur - avg), 2);
		}
		std = powf((std / (float)opts.o_samples), 0.5f);

		/* print final results */
		if (min_tv.tv_sec != 0) sprintf(timestr1, "%d%06d", min_tv.tv_sec, min_tv.tv_usec);
		else sprintf(timestr1, "%d", min_tv.tv_usec);
		if (max_tv.tv_sec != 0) sprintf(timestr2, "%d%06d", max_tv.tv_sec, max_tv.tv_usec);
		else sprintf(timestr2, "%d", max_tv.tv_usec);
		printf("avg RTT %f us, std dev %f, min RTT %s us, max RTT %s us\n", avg, std, timestr1, timestr2); fflush(stdout);
		if (opts.o_output) { fprintf(opts.o_output, "avg RTT %f us, std dev %f, min RTT %s us max RTT %s us\n", avg, std, timestr1, timestr2); fflush(opts.o_output); }
	}  /* if initator */

	else {  /* not initiator, reflect incoming msg back on other port */
		for (;;) {
			cur_size = recvfrom(sock, buff, 65536, 0, (struct sockaddr *)&src, &fromlen);
			if (cur_size == SOCKET_ERROR) { fprintf(stderr, "ERROR: ");  PERROR((&opts), "recv"); EXIT(1); }

			cur_size = sendto(sock, buff, cur_size, 0, (struct sockaddr *)&out_sa,sizeof(out_sa));
			if (cur_size == SOCKET_ERROR) { fprintf(stderr, "ERROR: ");  PERROR((&opts), "send"); EXIT(1); }
		}  /* for ;; */
	}

	CLOSESOCKET(sock);

	exit(0);
}  /* main */
