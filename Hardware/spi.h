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
 * File:   store.h
 * Author: jacques
 *
 * Created on 12 juin 2015, 12:01
 */

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "hardware.h"
#include <stdint.h>
    
#define _spi_write_done()  (SPI_STAT & (1<<BIT_SRMPT))
// s�lection carte SD
#define _spi_disable()   SPI_STAT &= ~(1<<BIT_SPIEN)
#define _spi_enable()    SPI_STAT |= (1<<BIT_SPIEN)
//#define configure_sdc_ss()   SDC_TRIS &= ~(1<<SDC_SS_RP)
//#define select_card()       SDC_LAT &=~(1<<SDC_SS_RP)
//#define unselect_card()     SDC_LAT |= (1<<SDC_SS_RP)

#define SLOW_CLOCK (0)
#define FAST_CLOCK (1)

extern void spi_clock_speed(uint8_t speed);
extern uint8_t spi_write(uint8_t);


#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */

