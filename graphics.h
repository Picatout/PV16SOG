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
 * File:   graphics.h
 * Author: jacques
 *
 * Created on 5 juin 2015, 13:49
 */

#ifndef GRAPHICS_H
#define	GRAPHICS_H

#ifdef	__cplusplus
"C" {
#endif
#include <stdint.h>
#include <stdbool.h>
#include "text.h"

    void draw_line(int x1, int y1, int x2, int y2, color_t color);
    void draw_rect(int x1, int y1, int x2, int y2, color_t color);
    bool draw_sprite(int x, int y, int width, int height, const uint8_t* sprite);
    //sauvegarde le buffer vidéo dans la SRAM à l'adresse <address>
    void save_screen(uint16_t address);
    //rempli le buffer vidéo avec le contenu de la SRAM qui est à l'adressse <address>
    void restore_screen(uint16_t address);
    // fonctions dans vm_graphics.S
    void draw_pixel(int x, int y, int color);
    void scroll_up(unsigned int lines);
    void scroll_down(unsigned int lines);
    void cls();
    void xor_pixel(int x, int y,uint8_t xor_value);
    //int get_pixel(int x, int y);
    void gray_scale(int top, int height);

#ifdef	__cplusplus
}
#endif

#endif	/* GRAPHICS_H */

