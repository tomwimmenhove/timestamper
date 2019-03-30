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

#include <avr/io.h>

#include "cdce925.h"
#include "i2c_master.h"

static void write_reg(uint8_t reg, uint8_t data)
{
    i2c_writeReg(CDCE925_ADDR * 2, reg, &data, 1);
}

static uint8_t read_reg(uint8_t reg)
{
    uint8_t data;

    i2c_readReg(CDCE925_ADDR * 2, reg, &data, 1);

    return data;
}


static const uint8_t cdce925_regs[] = {
    // Wit h added 8MHz output
    0x01, 0x08, 0xb4, 0x02, 0x02, 0x50, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x19, 0x05, 0xff, 0x00, 0x02, 0x83, 0xff, 0x00, 0x02, 0x80, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x05, 0xfd, 0x25, 0x12, 0x02, 0xfd, 0x25, 0x12, 0x00,

    //0x01, 0x00, 0xb4, 0x01, 0x02, 0x50, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x00, 0xfa, 0x05, 0xe3, 0xe6, 0xfa, 0x05, 0xe3, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x05, 0xff, 0xc3, 0xe2, 0x02, 0xff, 0xc3, 0xe2, 0x00,

};

void cdce925_init()
{   
    for (int i = 0; i < sizeof(cdce925_regs); i++)
    {   
        write_reg(0x80 | i, cdce925_regs[i]);
    }
}

void cdce925_burn()
{
    write_reg(0x86, 0); // Reset write bit
    write_reg(0x86, 1); // Set write bit

    // wait until done
    while (read_reg(0x81) & 0x40 == 0x40)
    {
        // Wait...
    }
}

