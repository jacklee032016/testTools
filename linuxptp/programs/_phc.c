/*
* Common for PHC (Ptp Hardware Clock) access in Linux
* JL.
*/ 


static clockid_t clock_open(char *device, int *phc_index)
{
	struct sk_ts_info ts_info;
	char phc_device[19];
	int clkid;

	/* check if device is CLOCK_REALTIME */
	if (!strcasecmp(device, "CLOCK_REALTIME"))
		return CLOCK_REALTIME;

	/* check if device is valid phc device */
	clkid = phc_open(device);
	if (clkid != CLOCK_INVALID)
		return clkid;

	/* check if device is a valid ethernet device */
	if (sk_get_ts_info(device, &ts_info) || !ts_info.valid) {
		fprintf(stderr, "unknown clock %s: %m\n", device);
		return CLOCK_INVALID;
	}

	if (ts_info.phc_index < 0) {
		fprintf(stderr, "interface %s does not have a PHC\n", device);
		return CLOCK_INVALID;
	}

	sprintf(phc_device, "/dev/ptp%d", ts_info.phc_index);
	clkid = phc_open(phc_device);
	if (clkid == CLOCK_INVALID)
		fprintf(stderr, "cannot open %s: %m\n", device);
	*phc_index = ts_info.phc_index;
	return clkid;
}

