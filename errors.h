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
 * File:   errors.h
 * Author: jacques
 *
 * Created on 6 décembre 2015, 11:58
 */

#ifndef ERRORS_H
#define	ERRORS_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum ERROR_CODE {eERR_NONE,eERR_MEM,eERR_SDC,eERR_PARTITION,
        eERR_FAT,eERR_DIRECTORY,eERR_NOTFILE,eERR_OPENFAIL,eERR_SEEK,
        eERR_FILEREAD,eERR_FILEWRITE,eERR_FCREATE,eERR_FILE2BIG,
        eERR_ADDR,eERR_DIV0,eERR_DMA,eERR_MATH,eERR_STK,eERR_KBD} err_code_t;    


void error(err_code_t error_code);
void fatal(err_code_t error_code);
#ifdef	__cplusplus
}
#endif

#endif	/* ERRORS_H */

