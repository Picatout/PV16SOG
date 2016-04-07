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
 *  functions graphiques
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "Hardware/TVout.h"
#include "Hardware/spi_ram.h"
#include "graphics.h"
#include "Hardware/ps2_kbd.h"
#include "text.h"
#include "Hardware/systicks.h"
#include "stackvm.h"


void draw_line(int x1, int y1, int x2, int y2, color_t color){
    int dx, dy, dxabs, dyabs, i, px, py, sdx, sdy, x, y;

#define sign(x) ((x) > 0 ? 1: ((x) == 0 ? 0: (-1)))

    dx=x2-x1;
    dy=y2-y1;
    sdx=sign(dx);
    sdy=sign(dy);
    dxabs=abs(dx);
    dyabs=abs(dy);
    x=0;
    y=0;
    px=x1;
    py=y1;
    if (dxabs>dyabs){
        for (i=0;i<dxabs;i++){
            y += dyabs;
            if (y>dxabs){
                y -= dxabs;
                py += sdy;
            }
            px += sdx;
            draw_pixel(px,py,color);
        }
    }else{
        for (i=0;i<dyabs;i++){
            x += dxabs;
            if (x>=dyabs){
                x -= dyabs;
                px += sdx;
            }
            py += sdy;
            draw_pixel(px,py,color);
        }
    }
}//f()

void draw_rect(int x1, int y1, int x2, int y2, color_t color){
    int i,limit;
    
    if (x1<x2){
        i=x1;
        limit=x2;
    }else{
        i=x2;
        limit=x1;
    }
    for (;i<=limit;i++){
        draw_pixel(i,y1,color);
        draw_pixel(i,y2,color);
    }
    if (y1<y2){
        i=y1+1;
        limit=y2;
    }else{
        i=y2+1;
        limit=y1;
    }
    for (;i<limit;i++){
        draw_pixel(x1,i,color);
        draw_pixel(x2,i,color);
    }
}//f()

bool draw_sprite(int x, int y, int width, int height, const uint8_t* sprite){
    bool collision=false;
    int b,c,r, sprt_w;
    color_t scr_pixel,pixel_color;
    
    sprt_w=width>>1;
    if (width&1) sprt_w++;
    for (r=0;r<height;r++){
        if ((r+y)<0) continue;
        if ((r+y)>=VRES) break;
        for (c=0;c<width;c++){
            if ((c+x)<0)continue;
            if ((c+x)>=HRES) break;
            if ((c+x)&1){
                scr_pixel=video_buffer[(r+y)*BPL+((c+x)>>1)]&0xf;
            }else{
                scr_pixel=(video_buffer[(r+y)*BPL+((c+x)>>1)]>>4);
            }
            b=sprite[(sprt_w*r)+(c>>1)]; 
            if (c&1){
                pixel_color=b&0xf;
            }else{
                pixel_color=(b>>4)&0xf;
            }
            scr_pixel ^= pixel_color;
            collision = collision || !((pixel_color==bg_color) || ((pixel_color^scr_pixel)==bg_color));
            draw_pixel(c+x,r+y,scr_pixel);
        }
    }
    return collision;
}//f()

//sauvegarde le buffer vidéo dans la SRAM à l'adresse <address>
void save_screen(uint16_t address){
    cursor_t curpos;
    sram_write_block(address,(const uint8_t*)video_buffer,BPL*VRES);
    curpos=get_cursor();
    sram_write_word(address+BPL*VRES,(uint16_t)(curpos.col+(curpos.line<<8)));
}//f()

//rempli le buffer vidéo avec le contenu de la SRAM qui est à l'adressse <address>
void restore_screen(uint16_t address){
    uint16_t curpos;
    sram_read_block(address,(uint8_t*)video_buffer,BPL*VRES);
    curpos=sram_read_word(address+BPL*VRES);
    set_cursor(curpos&255,curpos>>8);
}//f()

