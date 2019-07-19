
/* Test program for SIOC{G,S}HWTSTAMP
 * refer to Documents/networking/timestamping.txt, section 3
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/net_tstamp.h>	/* hw timestamp_config etc. */
#include <linux/sockios.h>

#include <arpa/inet.h>	/* IPPROTO_UDP */

#include "extLog.h"

static int lookup_value(const char **names, int size, const char *name)
{
	int value;

	for (value = 0; value < size; value++)
		if (names[value] && strcasecmp(names[value], name) == 0)
			return value;

	return -1;
}

static const char *lookup_name(const char **names, int size, int value)
{
	return (value >= 0 && value < size) ? names[value] : NULL;
}

static void list_names(FILE *f, const char **names, int size)
{
	int value;

	for (value = 0; value < size; value++)
		if (names[value])
			fprintf(f, "    %s\n", names[value]);
}

static const char *tx_types[] = {
#define TX_TYPE(name) [HWTSTAMP_TX_ ## name] = #name
	TX_TYPE(OFF),
	TX_TYPE(ON),
	TX_TYPE(ONESTEP_SYNC)
#undef TX_TYPE
};
#define N_TX_TYPES ((int)(sizeof(tx_types) / sizeof(tx_types[0])))

static const char *rx_filters[] = {
#define RX_FILTER(name) [HWTSTAMP_FILTER_ ## name] = #name
	RX_FILTER(NONE),
	RX_FILTER(ALL),
	RX_FILTER(SOME),
	RX_FILTER(PTP_V1_L4_EVENT),
	RX_FILTER(PTP_V1_L4_SYNC),
	RX_FILTER(PTP_V1_L4_DELAY_REQ),
	RX_FILTER(PTP_V2_L4_EVENT),
	RX_FILTER(PTP_V2_L4_SYNC),
	RX_FILTER(PTP_V2_L4_DELAY_REQ),
	RX_FILTER(PTP_V2_L2_EVENT),
	RX_FILTER(PTP_V2_L2_SYNC),
	RX_FILTER(PTP_V2_L2_DELAY_REQ),
	RX_FILTER(PTP_V2_EVENT),
	RX_FILTER(PTP_V2_SYNC),
	RX_FILTER(PTP_V2_DELAY_REQ),
#undef RX_FILTER
};
#define N_RX_FILTERS ((int)(sizeof(rx_filters) / sizeof(rx_filters[0])))

static void usage(char *progm)
{
	EXT_ERRORF("Usage: %s if_name [tx_type rx_filter]\n"
	      "tx_type is any of (case-insensitive):", progm );
	list_names(stderr, tx_types, N_TX_TYPES);
	EXT_ERRORF("rx_filter is any of (case-insensitive):" );
	list_names(stderr, rx_filters, N_RX_FILTERS);
}

int main(int argc, char **argv)
{
	struct ifreq ifr;
	struct hwtstamp_config config;
	const char *name;
	int sock;

	if ((argc != 2 && argc != 4) || (strlen(argv[1]) >= IFNAMSIZ))
	{
		usage(argv[0] );
		return 2;
	}

	if (argc == 4)
	{
		config.flags = 0;
		config.tx_type = lookup_value(tx_types, N_TX_TYPES, argv[2]);
		config.rx_filter = lookup_value(rx_filters, N_RX_FILTERS, argv[3]);
		if (config.tx_type < 0 || config.rx_filter < 0)
		{
			usage(argv[0]);
			return 2;
		}
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);
//	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0)
	{
		perror("socket: %m");
		return 1;
	}

	strcpy(ifr.ifr_name, argv[1]);
	ifr.ifr_data = (caddr_t)&config;
	if (ioctl(sock, (argc == 2) ? SIOCGHWTSTAMP : SIOCSHWTSTAMP, &ifr))
	{
		EXT_ERRORF("ioctl: %m");
		return 1;
	}

	EXT_INFOF("%s flags = %#x", (argc==2)?"Get(SIOCGHWTSTAMP)":"Set(SIOCSHWTSTAMP)", config.flags);
	name = lookup_name(tx_types, N_TX_TYPES, config.tx_type);
	if (name)
	{
		EXT_INFOF("tx_type = %s", name);
	}
	else
	{
		EXT_INFOF("tx_type = %d", config.tx_type);
	}
	
	name = lookup_name(rx_filters, N_RX_FILTERS, config.rx_filter);
	if (name)
	{
		EXT_INFOF("rx_filter = %s", name);
	}
	else
	{
		EXT_INFOF("rx_filter = %d", config.rx_filter);
	}

	return 0;
}

