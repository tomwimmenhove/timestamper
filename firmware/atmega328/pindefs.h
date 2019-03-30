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

#ifndef PINDEFS_H
#define PINDEFS_H

#define SCLK_PORT   PORTB
#define SCLK_DDR    DDRB
#define SCLK_MASK   _BV(5)

#define SS_PORT PORTB
#define SS_DDR  DDRB
#define SS_MASK _BV(2)

#define MR_PORT PORTC
#define MR_DDR  DDRC
#define MR_MASK _BV(3)

#define CS_PORT PORTD
#define CS_DDR  DDRD
#define CS_MASK _BV(3)

#define RESET_CAPT_PORT PORTD
#define RESET_CAPT_DDR  DDRD
#define RESET_CAPT_MASK _BV(4)

#endif // PINDEFS_H
