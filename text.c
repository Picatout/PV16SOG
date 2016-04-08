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
 * console.c
 *
 * Created: 2014-09-30 22:31:27
 *  Author: Jacques Deschênes
 */ 

#include <string.h>
#include <stdio.h>

#include "Hardware/hardware.h"
#include "Hardware/TVout.h"
#include "graphics.h"
#include "text.h"
#include "Hardware/systicks.h"
#include "Hardware/spi_ram.h"
#include "Hardware/ps2_kbd.h"
#include "Hardware/tones.h"
#include "font6x8.h"
#include "stackvm.h"	

#define TAB_WIDTH 4

int xpos=0, ypos=0;
uint8_t fg_color=cWHITE;
uint8_t bg_color=cBLACK;

static bool cursor_visible=false;
static cursor_shape_t cursor_shape=cVLINE;



//void new_line(){
//    if ((ypos+CHAR_HEIGHT)<=(VRES-CHAR_HEIGHT+1)){
//        clreol();
//        ypos+=CHAR_HEIGHT;
//    }else{
//        scroll_up(CHAR_HEIGHT);
//    }
//	xpos=0;
//}

static void draw_char(unsigned x, unsigned y, char c){
    int x0,y0;
    uint8_t row;
    __eds__ const uint8_t *glyph;
    
    glyph =&font_6x8[(c-32)*CHAR_HEIGHT];
    for (y0=0;y0<CHAR_HEIGHT;y0++){
        row= *glyph++;
        for (x0=x;x0<x+CHAR_WIDTH;x0++){
            if (row & 128){
                draw_pixel(x0,y+y0,fg_color);
            }
            else{
                draw_pixel(x0,y+y0,bg_color);
            }
            row <<= 1;
        }
    }
}//f()

static void cursor_right(){
    xpos+=CHAR_WIDTH;
    if (xpos>(HRES-CHAR_WIDTH+1)){
      new_line();
    }
}//f

static void cursor_left(){
    xpos-=CHAR_WIDTH;
    if (xpos<0){
        if (ypos>=CHAR_HEIGHT){
            ypos-=CHAR_HEIGHT;
        }else{
            scroll_down(CHAR_HEIGHT);
        }
        xpos=((int)HRES/CHAR_WIDTH-1)*CHAR_WIDTH;
    }
}//f


 void put_char(uint8_t c){
    switch (c){
        case CR:
        case LF:
            new_line();
            break;
        case NP:
            cls();
            break;
        case TAB:
            draw_char(xpos,ypos,' ');
            while ((xpos/CHAR_WIDTH)%TAB_WIDTH){
                draw_char(xpos,ypos,' ');
                xpos+=CHAR_WIDTH;
            }
            break;
        case BEL:
            beep(500,100,true);
            break;
        default:
            if ((c>31) && (c<(FONT_SIZE+32))){
                draw_char(xpos,ypos,c);
                cursor_right();
            }
    }//switch
}
 
cursor_t get_cursor(){
    cursor_t cursor;
    cursor.col=xpos/CHAR_WIDTH;
    cursor.line=ypos/CHAR_HEIGHT;
	return cursor;
}

//positionne le curseur texte
void set_cursor(uint8_t col, uint8_t line){
    if (col>=CHAR_PER_LINE) col=CHAR_PER_LINE-1;
    if (line>=LINE_PER_SCREEN) line=LINE_PER_SCREEN-1;
    xpos=col*CHAR_WIDTH;
    ypos=line*CHAR_HEIGHT;
}//f()


void text_clear_line(uint16_t line){
    memset((void*)(video_buffer+line*CHAR_HEIGHT*BPL),(bg_color<<4)+bg_color,BPL*CHAR_HEIGHT);
}//f()


void print(const char* str){
	while (*str) put_char(*str++);
}

void print_int(int32_t n, int width){
#define MAX_WIDTH 12
	uint8_t nstr[MAX_WIDTH];
	int i=MAX_WIDTH-1;
    bool negative=false;
    
    if (n<0){
        negative=true;
        n=-n;
    }
    if (width>MAX_WIDTH)width=MAX_WIDTH;
	do {
		nstr[i--]=n%10+'0';
		n/=10;
        width--;
	}while (n);
    if (negative){
        nstr[i--]='-';
        width--;
    }
	while (width>0){
        put_char(' ');
        width--;
    }
	i++;
	while (i<MAX_WIDTH) put_char(nstr[i++]);
}

void print_hex(uint16_t n, int width){
	uint8_t hex[4];
	uint8_t i=3,h;
	
    if (width>4) width=4;
	do{
		h=n&0xf;
		if (h<10) hex[i]=h+'0'; else hex[i]=h+'A'-10;
		n>>=4;
		i--;
		width--;
	}while (n);
	put_char(' ');
	while (width>0){hex[i--]='0'; width--;}
	i++;
	while (i<4) put_char(hex[i++]);
}

void update_cursor(){
    int i,j;
    if (cursor_shape==cVLINE){
        for (i=ypos;i<ypos+CHAR_HEIGHT;i++)
            xor_pixel(xpos,i,fg_color);
    }else{
        for (i=ypos;i<ypos+CHAR_HEIGHT;i++){
            for(j=xpos;j<xpos+CHAR_WIDTH;j++)
                xor_pixel(j,i,fg_color);
        }
    }
    cursor_visible = !cursor_visible;
}//f()

//affiche ou cache le curseur
void cursor_show(bool visible){
    if (cursor_visible!=visible)
        update_cursor();
}//f()

// attend la saisie d'une touche en affichant un curseur
uint8_t wait_key(){
    uint8_t key;

    pause_timer=333;
    while (!(key=kbd_get_key())){
        if (!pause_timer){
            update_cursor();
            pause_timer=333;
        }
    }
    cursor_show(false);
    pause_timer=0;
    return key;
}//f()


//détermine la forme du curseur
void set_cursor_shape(cursor_shape_t shape){
    cursor_shape=shape;
}//f()

// affiche un message et attend une touche en réponse
// n'accepte que les touches qui sont dans la chaîne key_set ou
// la touche <ESC>,
// si key_set=="*" accepte n'importe quelle touche.
char prompt(const char *msg, const char *key_set){
    uint8_t key=255;
    
    print(msg);
    print("? ");
    if (!strcmp(key_set,"*")){
        key=wait_key();
    }else while (!(strchr(key_set,key)||key==ESC)){
        key=wait_key();
    }
    new_line();
    return key;
}//f()

void prompt_continue(){
    prompt("\nany key...","*");
}


#define HIST_SIZE (10)
static char history[HIST_SIZE][CHAR_PER_LINE];
static int last=0;
static int circle=0;
static bool insert_mode=true; //true=insert, false=overwrite

//retourne le nombre de caractères lus.
// <ENTER> termine la ligne
uint8_t readln(char *buffer, uint8_t buff_len){
    uint8_t len=0,first_col,first_line,pos=0;
    uint8_t c, cline;
    
    first_col=text_colon();
    first_line=text_line();
    buffer[len]=0;
    while (1){
        c=wait_key();
        switch(c){
            case VK_UP:
                strcpy(buffer,history[last--]);
                if (last<0) last=HIST_SIZE-1;
                len=strlen(buffer);
                pos=len;
                if (text_line()>first_line) text_clear_line(text_line());
                set_cursor(first_col,first_line);
                print(buffer);
                clreol();
                break;
            case VK_DOWN:
                strcpy(buffer,history[last++]);
                if (last>(HIST_SIZE-1)) last=0;
                len=strlen(buffer);
                pos=len;
                if (text_line()>first_line) text_clear_line(text_line());
                set_cursor(first_col,first_line);
                print(buffer);
                clreol();
                break;
            case VK_LEFT:
                if (pos){
                    pos--;
                    cursor_left();
                }
                break;
            case VK_RIGHT:
                if (pos<len){
                    pos++;
                    cursor_right();
                }
                break;
            case VK_INSERT:
                insert_mode=!insert_mode;
                if (insert_mode) 
                    cursor_shape=cVLINE;
                else//écrasement
                    cursor_shape=cBLOCK;
                break;
            case BACKSPACE:
                if (len && pos){
                    memmove(&buffer[pos-1],&buffer[pos],len-pos);
                    len--;
                    buffer[len]=0;
                    pos--;
                    cursor_left();
                    clreol();
                    print(&buffer[pos]);
                }
                break;
            case VK_DELETE:
                if (len && pos<len){
                    memmove(&buffer[pos],&buffer[pos+1],len-pos-1);
                    len--;
                    buffer[len]=0;
                    clreol();
                    print(&buffer[pos]);
                }
                break;
            case VK_ENTER:
                buffer[len]=0;
                strcpy(history[circle],buffer);
                last=circle;
                circle++;
                if (circle>(HIST_SIZE-1)) circle=0;
                while (pos<len){
                    cursor_right();
                    pos++;
                }
                new_line();
                return len;
            default:
                if (insert_mode || pos==len){
                    if ((len<(buff_len-1)) && (c>=32) && (c<(FONT_SIZE+32))){
                        if (pos<len){
                            memmove(&buffer[pos+1],&buffer[pos],len-pos);
                            buffer[pos]=c;
                            len++;
                            buffer[len]=0;
                            clreol();
                            cline=text_line();
                            print(&buffer[pos]);
                            pos++;
                            if (cline==first_line)
                                set_cursor(first_col+pos,cline);
                            else
                                set_cursor(pos,cline);
                        }else{
                            buffer[pos++]=c;
                            len++;
                            put_char(c);
                        }
                    }else{
                        beep(500,100,false);
                    }
                }else{
                    if (c>=32 && c<(FONT_SIZE+32)){
                        buffer[pos++]=c;
                        put_char(c);
                    }
                }
                break;
        }//switch
    }//while
    return len;
}//f()

int16_t pstrcpy(char* dest, __eds__ const char *src){
    char *start=dest;
    while (*src) *dest++=*src++;
    *dest=0;
    return dest-start;
}//f()

// comparaison chaine en RAM avec chaine en FLASH
int pstrcmp(const unsigned char* str, __eds__ const unsigned char *pstr){
    while (*str || *pstr){
        if (*str<*pstr) return -1;
        if (*str>*pstr) return 1;
        str++;
        pstr++;
    }
    return 0;
}

// in situ capitalisation
void uppercase(char *str){
    while (*str){
        if ((*str>='a') && (*str<='z')){
            *(str++)-=32;
        }else{
            str++;
        }
    }
}

