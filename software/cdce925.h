/* 
 * This file is part of the TimeStamper distribution (https://github.com/tomwimmenhove/timestamper)
 * Copyright (c) 2019 Tom wimmenhove
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CDCE925_H
#define CDCE925_H

#define PLL_OSC_MIN 80000000.0
#define PLL_OSC_MAX 230000000.0

#define PLL_RANGE_LT_125	0x00
#define PLL_RANGE_LTE_150	0x01
#define PLL_RANGE_LTE_175	0x02
#define PLL_RANGE_GT_175	0x03


struct cdce_nrqp
{
	int n, r, q, p;
};

uint8_t cdce925_get_reg(int fd, uint8_t reg);
void cdce925_set_reg(int fd, uint8_t reg, uint8_t x);

int cdce925_calc_m(int p, int q, int r, int n);

double cdce925_find_best(struct cdce_nrqp* nrqp, double ref, double f);

void cdce925_get_nrqp(struct cdce_nrqp* nrqp, int fd, int pll_id);
void cdce925_set_nrqp(struct cdce_nrqp* nrqp, int fd, int pll_id, int range);

int cdce925_getrange(double f);

int cdce925_get_clockdiv(int fd);
void cdce925_set_clockdiv(int fd, uint8_t div);

int cdce925_get_auxdiv(int fd);
void cdce925_set_auxdiv(int fd, uint8_t div);

#endif /* CDCE925_H */
