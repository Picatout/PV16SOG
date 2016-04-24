/*
 * PV16SOG retro-computer 
 * specifications:
 *  MCU: PIC24EP512MC202
 *  video out: 
 *     - NTSC signal
 *     - graphics 240x170 16 shades of gray
 *     - text 21 lines 40 colons.
 *  storage:
 *     - SDcard
 *     - temporary: SPI RAM 64Ko
 *  input:
 *     - PS/2 keyboard
 *     - Atari 2600 joystick
 * sound:
 *     - single tone or white noise.
 * software:
 *     - small command shell
 *     - virtual machine derived from SCHIP
 *     - editor
 *     - assembler for the virtual machine
 * 
 * Copyright 2015, 2016, Jacques Deschenes
 * 
 * This file is part of PV16SOG project.
 * 
 * ***  LICENCE ****
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; See 'copying.txt' in root directory of source.
 * If not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *  
 * to contact the author:  jd_temp@yahoo.fr
 * 
 */

/*
 */

#include "spi.h"
#include "spi_ram.h"
#include "../FAT/sd_raw_config.h"

void store_init(){
    sram_set_mode(MODE_SEQ);
}//f();

void spi_clock_speed(uint8_t speed){
    _spi_disable();
    SPI_CON1 &= 0xffe0;
    if(speed==SLOW_CLOCK){
        SPI_CON1|=(6<<BIT0_SPRE);
    }else{
        SPI_CON1|=(5<<BIT0_SPRE)|(3<<BIT0_PPRE);
    }
    _spi_enable();
}//f()

inline uint8_t spi_write(uint8_t b){
    SPI_TX_BUF=b;
    while (!_spi_rxbuff_full());
    return SPI_RX_BUF;
}//f()


