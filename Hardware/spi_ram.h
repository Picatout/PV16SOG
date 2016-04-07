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
 * File:   spi_ram.h
 * Author: jacques
 *
 * Created on 9 juin 2015, 21:31
 */

#ifndef SPI_RAM_H
#define	SPI_RAM_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdint.h>

#include "hardware.h"
#include "spi.h"

#define SRAM_SIZE (65536)  //23LC512
// code de fonctions de la SRAM
#define SRAM_READ (3)
#define SRAM_WRITE (2)
#define SRAM_EDIO (0x3b)
#define SRAM_EQIO (0x38)
#define SRAM_RSTIO (0xff)
#define SRAM_RDMR   (5)
#define SRAM_WRMR  (1)

#define MODE_BYTE  (0)
#define MODE_PAGE  (2<<6)
#define MODE_SEQ   (1<<6)


    // définie le mode d'opération de la RAM
    extern void sram_set_mode(uint8_t mode);
    extern uint8_t sram_get_mode();
    // écris un octet dans RAM
    extern void sram_write_byte(uint16_t address, uint8_t byte);
    // lit un octet de la RAM
    extern uint8_t sram_read_byte(uint16_t address);
    // écris un mot de 16 bits
    extern void sram_write_word(uint16_t address, uint16_t word);
    // lit un mot de 16 bits
    extern uint16_t sram_read_word(uint16_t address);
    //écris un bloc d'octets dans la RAM
    extern void sram_write_block(uint16_t address, const uint8_t *data, uint16_t count);
    //lit un bloc d'octets de la RAM
    extern void sram_read_block(uint16_t address, uint8_t *data, uint16_t count);
    // remet à zéro tout le contenu de la RAM
    extern void sram_clear();
    // met à zéro un block de la RAM
    extern void sram_clear_block(uint16_t address, uint16_t size);
#ifdef	__cplusplus
}
#endif

#endif	/* SPI_RAM_H */

