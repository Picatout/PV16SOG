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
 * File:   ps2_kbd.h
 * Author: jacques
 *
 * Created on 13 juin 2015, 20:18
 */

#ifndef PS2_KBD_H
#define	PS2_KBD_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef __asm    
#include <stdint.h>
#include <stdbool.h>
#endif
#include "hardware.h"

#ifndef __asm
// structure pour la table de transcription des codes clavier
typedef struct scan2key{
	 short code;
	 short ascii;
}t_scan2key;
#endif

// commandes contrôle clavier
#define KBD_RESET 0xFF  // commande RAZ clavier
#define KBD_LED 0xED    // commande de contrôle des LEDS

#define XT_BIT	(1<<8)  // indicateur code étendu
#define REL_BIT (1<<15) // indicateur touche relâchée
#define FN_BIT (1<<9) // indicateur touche non ASCII (F* PRN, PAUSE, SCROLL, flèches, etc)

// touches spéciales
#define ENTER           0x5A
#define CAPS_LOCK		0x58
#define NUM_LOCK		0x77
#define SCROLL_LOCK		0x7E
#define LSHIFT			0x12
#define RSHIFT          0x59
#define CTRL            0x14
#define LCTRL			0x14
#define RCTRL           (0x14)
#define ALT             0x11
#define LALT			0x11
#define RALT            0x11
#define BKSP            0x66
#define KEY_ESC			0x76
#define F1			0x05
#define F2			(0x06)
#define F3			(0x04)
#define F4			(0x0c)
#define F5			(0x03)
#define F6			(0x0b)
#define F7			(0x83)
#define F8			(0x0a)
#define F9			(0x01)
#define F10			(0x09)
#define F11			(0x78)
#define F12			(0x07)
#define PRN			(0x12)   // touche PrtSc enfoncée 4 codes: 0xe012e07c, relâchée 6 codes: 0xe0f07ce0f012
#define PAUSE			(0xe1)   // touche Pause séquence de 8 codes 0xe11477e1f014f077
#define KEY_UP  		(0x75)
#define KEY_DOWN		(0x72)
#define KEY_LEFT		(0x6B)
#define KEY_RIGHT		(0x74)
#define INSERT			(0x70)
#define HOME			(0x6c)
#define	PGUP			(0x7d)
#define PGDN			(0x7a)
#define	DEL			(0x71)
#define END			(0x69)
#define KPDIV		(0x4a)
#define KPMUL			0x7c
#define KPMINUS			0x7b
#define KPPLUS			0x79
#define KPENTER			(0x5a)
#define KPDOT			0x71
#define KP0			0x70
#define KP1			0x69
#define KP2			0x72
#define KP3			0x7a
#define KP4			0x6b
#define KP5			0x73
#define KP6			0x74
#define KP7			0x6c
#define KP8			0x75
#define KP9			0x7d

//touches virtuelles
#define VK_BACK 8
#define VK_TAB 9
#define VK_ESC 27
#define VK_ENTER '\r'
#define VK_F1 128
#define VK_F2 129
#define VK_F3 130
#define VK_F4 131
#define VK_F5 132
#define VK_F6 133
#define VK_F7 134
#define VK_F8 135
#define VK_F9 136
#define VK_F10 138
#define VK_F11 139
#define VK_F12 140
#define VK_UP  141
#define VK_DOWN 142
#define VK_LEFT 143
#define VK_RIGHT 144
#define VK_HOME 145
#define VK_END  146
#define VK_PGUP 147
#define VK_PGDN 148
#define VK_INSERT 149
#define VK_DELETE 150


#define F_NUM  1 // bit indicateur numlock dans kbd_leds
#define F_CAPS 2 // bit indicateur capslock dans kbd_leds
#define F_LSHIFT 4  // bit indicateur shift dans kbd_leds
#define F_RSHIFT 8 // bit indicateur touche contrôle
#define F_SHIFT (F_LSHIFT|F_RSHIFT)
#define F_LCTRL 16  // bit indiateur touche alt enfoncée
#define F_RCTRL 32
#define F_CTRL (F_LCTRL|F_RCTRL)
#define F_LALT 64
#define F_ALTCHAR  128
#define F_ALT (F_LALT|F_ALTCHAR)


#ifndef __asm
extern volatile uint8_t key_state; // état des touches d'alteration: shift, ctrl ,alt

uint16_t  kbd_get_scancode();
uint8_t kbd_get_key();
uint8_t kbd_wait_key();
void kbd_send(uint8_t code);
int kbd_queue_empty();
bool kbd_reset();
bool kbd_ready();
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* PS2_KBD_H */

