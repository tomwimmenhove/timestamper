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

#ifndef SPI2_H
#define SPI2_H

/* Stolen from the Arduino SPI.h header */
static inline uint8_t spi_inout(uint8_t data)
{
	SPDR = data;
	/*
	 * The following NOP introduces a small delay that can prevent the wait
	 * loop form iterating when running at the maximum speed. This gives
	 * about 10% more speed, even if it seems counter-intuitive. At lower
	 * speeds it is unnoticed.
	 */
	__asm__ volatile("nop");
	while (!(SPSR & _BV(SPIF))) ; // wait
	return SPDR;
}


#endif // SPI2_H

