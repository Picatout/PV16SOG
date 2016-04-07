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
 * File:   TVout.h
 * Author: jacques
 *
 * Created on 5 juin 2015, 09:30
 */
/*
 *  PB0:PB3  utilisé comme sortie vidéo NTSC
 *  PB4 utilisé comme sortie synchronisation NTSC
 *  TIMER2, OC1 et OC2 utilisé pour générer pour
 *  synchronisation
 *   __T2Interrupt priorité 5
 *   __OC2Interrupt priorité 5
 *
 */


#ifndef TVOUT_H
#define	TVOUT_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "ntsc_const.h"
#define _disable_video() {dis_video=1;IEC0bits.OC2IE=0;}
#define _enable_video() {dis_video=0;}
    
extern volatile unsigned char video_buffer[TV_BUFFER];
extern volatile unsigned char dis_video;

#ifndef __asm
//change la couleur de la police
//extern void tv_font_color(unsigned char color);
//change la couleur de fond de la police
//extern void tv_back_color(unsigned char color);
//affiche un caractère à l'écran en position x,y
//extern void tv_char(int x, int y, char c);
#endif // __asm

#ifdef	__cplusplus
}
#endif

#endif	/* TVOUT_H */

