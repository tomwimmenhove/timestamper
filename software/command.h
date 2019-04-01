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

#ifndef COMMAND_H
#define COMMAND_H

uint16_t wait_reply(int fd, uint32_t ev);
uint16_t send_command(int fd, uint32_t cmd);

static inline uint32_t unpack(uint8_t* data)
{
	return ((data[0] & 0x7f) << 21) | (data[1] << 14) | (data[2] << 7) | data[3];
}

ssize_t packet_out(int fd, uint32_t x);

#endif /* COMMAND_H */
