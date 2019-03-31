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

#ifndef PRINTF_H
#define PRINTF_H

static inline void uart_putchar(char c)
{
	loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
	UDR0 = c;
}

static inline char uart_getchar(void)
{
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data available. */
	return UDR0;
}

static inline char uart_haschar()
{
	return (UCSR0A & _BV(RXC0)) != 0;
}

static inline void uart_puts(const char* s)
{
	char c;
	while ((c = *s++))
	{
		uart_putchar(c);
	}
}

void printf_init();


#endif // PRINTF_H

