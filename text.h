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
 * text.h
 *
 * Created: 2014-09-30 22:31:06
 *  Author: Jacques Deschênes
 */ 


#ifndef TEXT_H_
#define TEXT_H_

#ifndef __asm
#include <stdint.h>
#include <stdbool.h>
#include "Hardware/ntsc_const.h"
#endif // ~__asm



// caractères ASCII
#define ESC 27
#define BACKSPACE 8
#define TAB 9
#define CR 13
#define LF 10
#define BEL 7
#define NP  12
#define FF  12


//#define FONT_ASCII  (2)
#define CHAR_PER_LINE (int)(HRES/CHAR_WIDTH)
#define LINE_PER_SCREEN (int)(VRES/CHAR_HEIGHT)

#define RECTANGLE   (127)
#define RIGHT_ARROW (128)
#define LEFT_ARROW  (129)
#define UP_ARROW    (130)
#define DOWN_ARROW  (131)
#define ROUND       (132)

#ifndef __asm
typedef enum CURSOR_SHAPE{cBLOCK, cVLINE} cursor_shape_t;

typedef struct text_cursor{
    uint16_t col:8;
    uint16_t line:8;
}cursor_t;

typedef enum COLORS {cBLACK,cDARK_GRAY,cGRAY2,cGRAY3,cGRAY4,cGRAY5,cGRAY6,cGRAY7,cGRAY8,
cGRAY9,cGRAY10,cGRAY11,cGRAY12,cGRAY13,cLIGHT_GRAY,cWHITE} color_t;

extern uint8_t fg_color;
extern uint8_t bg_color;
extern int xpos;
extern int ypos;

//définie la couleur de fond d'écran.
#define set_back_color(new_color)  bg_color=new_color
//void set_back_color(color_t new_color);
//retourne la couleur de fond
#define get_back_color() bg_color
//color_t get_back_color();
//définie la couleur des caractères
#define set_font_color(new_color)  fg_color=new_color
//void set_font_color(color_t new_color);
//retourne le couleur de la police
#define get_font_color()  fg_color
//color_t get_font_color();
// efface l'écran met le curseur  à 0,0
//void cls();
//imprime un caractère à l'écran
void put_char(uint8_t c);
// retourne la position du curseur texte
cursor_t get_cursor();
//positionne le curseur texte
void set_cursor(uint8_t col, uint8_t line);
//ligne du curseur texte
uint8_t text_line();
//colonne du curseur texte
uint8_t text_colon();
//efface ligne de texte au complet
void text_clear_line(uint16_t line);
//positionne le curseur texte
//au début de la ligne suivante.
void new_line();
//imprime une chaine à l'écran
void print(const char* str);
// imprime un entier
void print_int(int32_t n, int width);
// imprime un entier en hexadécimal
void print_hex(uint16_t n, int width);
//efface la ligne à partir de la position du curseur
void clreol();
//affiche un message d'erreur en préservant l'écran
//void print_error(const char* msg);
// attend la saisie d'une touche en affichant un curseur
uint8_t wait_key();
//détermine la forme du curseur
void set_cursor_shape(cursor_shape_t shape);
// affiche ou cache le curseur
void cursor_show(bool visible);
// pause une question et attend la réponse
char prompt(const char *msg, const char *key_set);
// attend une touche pour continuer
void prompt_continue();
//lecture d'une ligne de texte
uint8_t readln(char *buffer, uint8_t buff_len);
//copie une chaîne de caractère de FLASH->RAM
int16_t pstrcpy(char* dest, __eds__ const char * src);
// comparaison chaine en RAM avec chaine en FLASH
int pstrcmp(const unsigned char* str, __eds__ const unsigned char *pstr);
// in situ capitalisation chaîne
void uppercase(char *str);
#endif // ~__asm
#endif /* TEXT_H_ */
