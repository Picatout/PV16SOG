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
 * File:   tests.h
 * Author: jacques
 *
 * Created on 22 juin 2015, 16:24
 */

#ifndef TESTS_H
#define	TESTS_H

#ifdef	__cplusplus
extern "C" {
#endif
#ifdef INCLUDE_TEST
extern void debug_save(unsigned val);
extern void debug_print();
extern void char_per_second();
extern void scale_up();
extern void random_lines();
extern void cls_speed();
extern void sram_test();
extern void joystick_test();
extern void kbd_test();
extern void sdc_test();
extern void sprite_test();
extern void img_test(const char* name, int lx, int ty);
extern void pixels_test();
extern const char texte[];
#endif 


#ifdef	__cplusplus
}
#endif

#endif	/* TESTS_H */

