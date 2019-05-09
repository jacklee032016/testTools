/**
 * @file filter.c
 * @note Copyright (C) 2013 Miroslav Lichvar <mlichvar@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <string.h>

#include "ptpCompact.h"
#include "tmv.h"
#include "filterPrivate.h"

/* Implements a moving median */
struct mmedian {
	struct PtpFilter filter;
	int cnt;
	int len;
	int index;
	/* Indices sorted by value. */
	int *order;
	/* Values stored in circular buffer. */
	tmv_t *samples;
};

static void __mmedianDestroy(struct PtpFilter *_filter)
{
	struct mmedian *m = container_of(_filter, struct mmedian, filter);
	free(m->order);
	free(m->samples);
	free(m);
}

static tmv_t __mmedianSample(struct PtpFilter *_filter, tmv_t sample)
{
	struct mmedian *m = container_of(_filter, struct mmedian, filter);
	int i;

	m->samples[m->index] = sample;
	if (m->cnt < m->len) {
		m->cnt++;
	} else {
		/* Remove index of the replaced value from order. */
		for (i = 0; i < m->cnt; i++)
			if (m->order[i] == m->index)
				break;
		for (; i + 1 < m->cnt; i++)
			m->order[i] = m->order[i + 1];
	}

	/* Insert index of the new value to order. */
	for (i = m->cnt - 1; i > 0; i--) {
		if (tmv_cmp(m->samples[m->order[i - 1]],
			    m->samples[m->index]) <= 0)
			break;
		m->order[i] = m->order[i - 1];
	}
	m->order[i] = m->index;

	m->index = (1 + m->index) % m->len;

	if (m->cnt % 2)
		return m->samples[m->order[m->cnt / 2]];
	else
		return tmv_div(tmv_add(m->samples[m->order[m->cnt / 2 - 1]],
				       m->samples[m->order[m->cnt / 2]]), 2);
}

static void __mmedianReset(struct PtpFilter *_filter)
{
	struct mmedian *m = container_of(_filter, struct mmedian, filter);
	m->cnt = 0;
	m->index = 0;
}

struct PtpFilter *_mmedianCreate(int length)
{
	struct mmedian *m;

	if (length < 1)
		return NULL;
	m = calloc(1, sizeof(*m));
	if (!m)
		return NULL;
	m->filter.destroy = __mmedianDestroy;
	m->filter.sample = __mmedianSample;
	m->filter.reset = __mmedianReset;
	m->order = calloc(1, length * sizeof(*m->order));
	if (!m->order) {
		free(m);
		return NULL;
	}
	m->samples = calloc(1, length * sizeof(*m->samples));
	if (!m->samples) {
		free(m->order);
		free(m);
		return NULL;
	}
	m->len = length;
	return &m->filter;
}

/* Implements a moving average */
struct mave
{
	struct PtpFilter filter;
	int cnt;
	int len;
	int index;
	tmv_t sum;
	tmv_t *val;
};

static void __maveDestroy(struct PtpFilter *_filter)
{
	struct mave *m = container_of(_filter, struct mave, filter);
	free(m->val);
	free(m);
}

static tmv_t __maveAccumulate(struct PtpFilter *_filter, tmv_t val)
{
	struct mave *m = container_of(_filter, struct mave, filter);

	m->sum = tmv_sub(m->sum, m->val[m->index]);
	m->val[m->index] = val;
	m->index = (1 + m->index) % m->len;
	m->sum = tmv_add(m->sum, val);
	if (m->cnt < m->len) {
		m->cnt++;
	}
	return tmv_div(m->sum, m->cnt);
}

static void __maveReset(struct PtpFilter *_filter)
{
	struct mave *m = container_of(_filter, struct mave, filter);

	m->cnt = 0;
	m->index = 0;
	m->sum = tmv_zero();
	memset(m->val, 0, m->len * sizeof(*m->val));
}

struct PtpFilter *_maveCreate(int length)
{
	struct mave *m;
	m = calloc(1, sizeof(*m));
	if (!m) {
		return NULL;
	}
	m->filter.destroy = __maveDestroy;
	m->filter.sample = __maveAccumulate;
	m->filter.reset = __maveReset;
	
	m->val = calloc(1, length * sizeof(*m->val));
	if (!m->val)
	{
		free(m);
		return NULL;
	}
	
	m->len = length;
	return &m->filter;
}

struct PtpFilter *filter_create(enum filter_type type, int length)
{
	switch (type) {
		case FILTER_MOVING_AVERAGE:
			return _maveCreate(length);
		case FILTER_MOVING_MEDIAN:
			return _mmedianCreate(length);
		default:
			return NULL;
	}
}

void filter_destroy(struct PtpFilter *filter)
{
	filter->destroy(filter);
}

tmv_t filter_sample(struct PtpFilter *filter, tmv_t sample)
{
	return filter->sample(filter, sample);
}

void filter_reset(struct PtpFilter *filter)
{
	filter->reset(filter);
}

