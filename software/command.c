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

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include "command.h"

ssize_t packet_out(int fd, uint32_t x)
{
	uint8_t data[4];

	data[0] = ((x >> 21) & 0x7f) | 0x80;
	data[1] =  (x >> 14) & 0x7f;
	data[2] =  (x >>  7) & 0x7f;
	data[3] =   x        & 0x7f;

	return write(fd, data, sizeof(data));
}

uint16_t wait_reply(int fd, uint32_t ev)
{
    uint8_t packet[4];
    int pos = 0;
    uint8_t buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0)
    {   
        for (int i = 0; i < n; i++)
        {   
            uint8_t b = buf[i];

            /* Start of 'packet' */
            if (b & 0x80)
            {   
                pos = 0;
            }
            if (pos < 4)
            {   
                packet[pos] = b;
            }

            pos++;

            if (pos == 4)
            {   
                uint32_t reply =  unpack(packet);

                if ((reply & 0xff0000) == (ev & 0xff0000))
                    return reply & 0xffff;
            }
        }
    }

    exit(n != -1 ? 0 : 1);
}

uint16_t send_command(int fd, uint32_t cmd)
{
    packet_out(fd, cmd);
    return wait_reply(fd, cmd);
}


