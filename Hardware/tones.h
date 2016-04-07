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
 * File:   tones.h
 * Author: jacques
 *
 * Created on 29 juin 2015, 21:13
 */

#ifndef TONES_H
#define	TONES_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "hardware.h"

#ifndef __asm    

//typedef enum SCALE{DO2D,RE2,RE2D,MI2,FA2,FA2D,SOL2,SOL2D,LA2,LA2D,SI2,
//                   DO3,DO3D,RE3,RE3D,MI3,FA3,FA3D,SOL3,SOL3D,LA3,LA3D,SI3,
//                   DO4,DO4D,RE4,RE4D,MI4,FA4,FA4D,SOL4,SOL4D,LA4,LA4D,SI4,
//                   DO5,DO5D,RE5,RE5D,MI5,FA5,FA5D,SOL5,SOL5D,LA5,LA5D,SI5,
//                   DO6
//                  }scale_t;
//
//                  
//#define SCALE_MIN DO2D                  
//#define SCALE_MAX DO6

#define SCALE_SIZE (48)
    extern volatile uint16_t tone_len;
    void beep(uint16_t freq, uint16_t length, bool wait);
    
#else
.equ SCALE_SIZE, 48                  
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* TONES_H */

