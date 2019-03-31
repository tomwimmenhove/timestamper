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

#include "cdce925_regs.h"

void write_cdce925_reg(uint8_t reg, uint8_t data)
{
    i2c_writeReg(CDCE925_ADDR * 2, reg, &data, 1);
}

uint8_t read_cdce925_reg(uint8_t reg)
{
    uint8_t data;

    i2c_readReg(CDCE925_ADDR * 2, reg, &data, 1);

    return data;
}

void cdce925_init()
{   
    for (int i = 0; i < sizeof(cdce925_regs); i++)
    {   
        write_cdce925_reg(0x80 | i, cdce925_regs[i]);
    }
}

void cdce925_burn()
{
    write_cdce925_reg(0x86, 0); // Reset write bit
    write_cdce925_reg(0x86, 1); // Set write bit

    // wait until done
    while (read_cdce925_reg(0x81) & 0x40 == 0x40)
    {
        // Wait...
    }
}

