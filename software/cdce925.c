#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "command.h"
#include "cdce925.h"

static int int_log2(int x)
{   
	int log = 0;

	while (x > 1)
	{   
		log++;
		x >>= 1;
	}

	return log;
}

static int cdce925_calc_pqr(struct cdce_nrqp* nrqp, int m)
{   
	int n_;

	nrqp->p = 4 - int_log2(nrqp->n / m);
	if (nrqp->p < 0)
		nrqp->p = 0;

	n_ = nrqp->n << nrqp->p;
	nrqp->q = n_ / m;

	if (nrqp->q < 16 || nrqp->q > 63)
	{   
		return -1;
	}

	nrqp->r = n_ - m * nrqp->q;

	return 0;
}

uint8_t cdce925_get_reg(int fd, uint8_t reg)
{
	return send_command(fd, 0x8030000 | reg);
}

void cdce925_set_reg(int fd, uint8_t reg, uint8_t x)
{
	send_command(fd, 0x8040000 | reg | (x << 8));
}

void cdce925_get_nrqp(struct cdce_nrqp* nrqp, int fd, int pll_id)
{
	int add = pll_id == 0 ? 0x00 : 0x10;

	int r18h = cdce925_get_reg(fd, 0x18 + add);
	int r19h = cdce925_get_reg(fd, 0x19 + add);
	int r1ah = cdce925_get_reg(fd, 0x1a + add);
	int r1bh = cdce925_get_reg(fd, 0x1b + add);

	nrqp->n = (r18h << 4) | ((r19h >> 4) & 0xf);
	nrqp->r = ((r19h & 0xf) << 5) | ((r1ah >> 3) & 0x1f);
	nrqp->q = ((r1ah & 0x7) << 3) | ((r1bh >> 5) & 0x7);
	nrqp->p = (r1bh >> 2) & 0x7;
}

void cdce925_set_nrqp(struct cdce_nrqp* nrqp, int fd, int pll_id, int range)
{
	int add = pll_id == 0 ? 0x00 : 0x10;

	int r18h = nrqp->n >> 4;
	int r19h = ((nrqp->n & 0xf) << 4) | (nrqp->r >> 5);
	int r1ah = ((nrqp->r & 0x1f) << 3) | (nrqp->q >> 3);
	int r1bh = ((nrqp->q & 0x7) << 5) | (nrqp->p << 2) | range;

	cdce925_set_reg(fd, 0x18 + add, r18h);
	cdce925_set_reg(fd, 0x19 + add, r19h);
	cdce925_set_reg(fd, 0x1a + add, r1ah);
	cdce925_set_reg(fd, 0x1b + add, r1bh);
}

int cdce925_getrange(double f)
{
	if (f < 125000000.0f)
		return PLL_RANGE_LT_125;
	else if (f <= 150000000.0f)
		return PLL_RANGE_LTE_150;
	else if (f <= 175000000.0f)
		return PLL_RANGE_LTE_175;
	else
		return PLL_RANGE_GT_175;
}

int cdce925_get_clockdiv(int fd)
{
	return cdce925_get_reg(fd, 0x03);
}

void cdce925_set_clockdiv(int fd, uint8_t div)
{
	cdce925_set_reg(fd, 0x03, div);
}

int cdce925_get_auxdiv(int fd)
{
	return cdce925_get_reg(fd, 0x27) & 0x7f;
}

void cdce925_set_auxdiv(int fd, uint8_t div)
{
	return cdce925_set_reg(fd, 0x27, div & 0x7f);
}

int cdce925_calc_m(int p, int q, int r, int n)
{
	int n_;

	n_ = n << p;
	return (n_ - r) / q;
}

double cdce925_find_best(struct cdce_nrqp* nrqp, double ref, double f)
{   
	int m;
	double f_osc;
	double dev, dev_min = f;
	double best_f = NAN;

	nrqp->p = nrqp->q = nrqp->r = -1;

	// For some reason, the TI tools seem to do it this (backwards iteration) way.
	for (m = 511; m > 0; m--)
	{   
		for (int n = 4094; n > 0; n--)
		{   
			f_osc = ref * n / m;
			if (f_osc < PLL_OSC_MIN || f_osc > PLL_OSC_MAX)
			{
				// XXX: Very un-optimized.
				continue;
			}

			dev = fabs(f - f_osc);

			if (dev < dev_min)
			{   
				nrqp->n = n;

				if (cdce925_calc_pqr(nrqp, m) == -1)
				{   
					continue;
				}

				best_f = f_osc;
				dev_min = dev;

				/* Exact match? */
				if (dev == 0.0f)
					return best_f;
			}
		}
	}

	return best_f;
}


