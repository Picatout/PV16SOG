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

#define abs(n) ((n)<0?-(n):(n))

//  REF: http://members.chello.at/~easyfilter/bresenham.html
void draw_line(int x0, int y0, int x1, int y1,color_t color)
{
   int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
   int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
   int err = dx+dy, e2; /* error value e_xy */

   for(;;){  /* loop */
      draw_pixel(x0,y0,color);
      if (x0==x1 && y0==y1) break;
      e2 = 2*err;
      if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
      if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
   }
}//line()

////  REF: http://members.chello.at/~easyfilter/bresenham.html
//void draw_circle(int xc, int yc, int r, color_t color)
//{
//   int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */
//   do {
//      draw_pixel(xc-x, yc+y,color); /*   I. Quadrant */
//      draw_pixel(xc-y, yc-x,color); /*  II. Quadrant */
//      draw_pixel(xc+x, yc-y,color); /* III. Quadrant */
//      draw_pixel(xc+y, yc+x,color); /*  IV. Quadrant */
//      r = err;
//      if (r <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
//      if (r > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
//   } while (x < 0);
//}//circle()

//ref: https://sites.google.com/site/ruslancray/lab/projects/bresenhamscircleellipsedrawingalgorithm/bresenham-s-circle-ellipse-drawing-algorithm
void draw_ellipse (int xc, int yc, int width, int height,color_t color)
{
    width/=2;
    height/=2;
    long a2 = (width * width);
    long b2 = (height * height);
    long fa2 = 4 * a2, fb2 = 4 * b2;
    long sigma;
    int x, y;

    /* first half */
    for (x = 0, y = height, sigma = 2*b2+a2*(1-2*height); b2*x <= a2*y; x++)
    {
        draw_pixel (xc + x, yc + y,color);
        draw_pixel (xc - x, yc + y,color);
        draw_pixel (xc + x, yc - y,color);
        draw_pixel (xc - x, yc - y,color);
        if (sigma >= 0)
        {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2 * ((4 * x) + 6);
    }

    /* second half */
    for (x = width, y = 0, sigma = 2*a2+b2*(1-2*width); a2*y <= b2*x; y++)
    {
        draw_pixel (xc + x, yc + y,color);
        draw_pixel (xc - x, yc + y,color);
        draw_pixel (xc + x, yc - y,color);
        draw_pixel (xc - x, yc - y,color);
        if (sigma >= 0)
        {
            sigma += fb2 * (1 - x);
            x--;
        }
        sigma += a2 * ((4 * y) + 6);
    }
}

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

void draw_sprite(int x, int y, int width, int height, const uint8_t* sprite, uint8_t *save_back){
    int c,r, sprt_w,xp,yp;
    color_t b,s, scr_pixel,pixel_color;

    sprt_w=width>>1;
    if (width&1) sprt_w++;
    for (r=0;r<height;r++){
        yp=r+y;
        if (yp<0) continue;
        if (yp>=VRES) break;
        for (c=0;c<width;c++){
            xp=c+x;
            if (xp<0)continue;
            if (xp>=HRES) break;
            if (xp&1){
                scr_pixel=video_buffer[yp*BPL+(xp>>1)]&0xf;
            }else{
                scr_pixel=video_buffer[yp*BPL+(xp>>1)]>>4;
            }
            b=sprite[(sprt_w*r)+(c>>1)]; 
            s=save_back[(sprt_w*r)+(c>>1)];
            if (c&1){
                pixel_color=b&0xf;
                s=(s&0xf0)|scr_pixel;
            }else{
                pixel_color=(b>>4);
                s=(s&0xf)|(scr_pixel<<4);
            }
            save_back[(sprt_w*r)+(c>>1)]=s;
            if (pixel_color){
                draw_pixel(xp,yp,pixel_color);
            }
        }//for 
    }//for
}//f()

// enlève le sprite de l'écran
void remove_sprite(int x,int y, int width, int height, const uint8_t* rest_back){
    int c,r, sprt_w,xp,yp;
    color_t b;

    sprt_w=width>>1;
    if (width&1) sprt_w++;
    for (r=0;r<height;r++){
        yp=r+y;
        if (yp<0) continue;
        if (yp>=VRES) break;
        for (c=0;c<width;c++){
            xp=c+x;
            if (xp<0)continue;
            if (xp>=HRES) break;
            b=rest_back[(sprt_w*r)+(c>>1)]; 
            if (c&1){
                draw_pixel(xp,yp,b&0xf);
            }else{
                draw_pixel(xp,yp,b>>4);
            }
        }//for 
    }//for
}//f

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

