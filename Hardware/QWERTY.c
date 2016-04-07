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
 * Fichier: QWERTY.c
 * Description: table de transcription scancode clavier pour mini clavier MCSaite
 * ref: http://www.adafruit.com/products/857 
 * Auteur: Jacques Deschênes
 * Date: 2013-09-08
 */

#include "QWERTY.h"

const t_scan2key qwerty[]={  // table de correspondance codes clavier -> ASCII (clavier QWERTY)
		{0x1c,'a'},
		{0x32,'b'},
		{0x21,'c'},
		{0x23,'d'},
		{0x24,'e'},
		{0x2b,'f'},
		{0x34,'g'},
		{0x33,'h'},
		{0x43,'i'},
		{0x3b,'j'},
		{0x42,'k'},
		{0x4b,'l'},
		{0x3a,'m'},
		{0x31,'n'},
		{0x44,'o'},
		{0x4d,'p'},
		{0x15,'q'},
		{0x2d,'r'},
		{0x1b,'s'},
		{0x2c,'t'},
		{0x3c,'u'},
		{0x2a,'v'},
		{0x1d,'w'},
		{0x22,'x'},
		{0x35,'y'},
		{0x1a,'z'},
		{0x45,'0'},
		{0x16,'1'},
		{0x1e,'2'},
		{0x26,'3'},
		{0x25,'4'},
		{0x2e,'5'},
		{0x36,'6'},
		{0x3d,'7'},
		{0x3e,'8'},
		{0x46,'9'},
		{0x29,' '},
		{0x4e,'-'},
		{0x55,'='},
		{0x0e,'`'},
		{0x0d,'\t'},
		{0x54,'['},
		{0x5b,']'},
		{0x4c,';'},
		{0x41,','},
		{0x49,'.'},
		{0x4a,'/'},
        {0x52,'\''},
		{0x5d,'\\'},
		{0x66,VK_BACK}, // BACKSPACE
		{0x0d,VK_TAB}, // TAB
		{0x5a,VK_ENTER}, // CR
		{0x76,VK_ESC}, // ESC
		{KP0,'0'},
		{KP1,'1'},
		{KP2,'2'},
		{KP3,'3'},
		{KP4,'4'},
		{KP5,'5'},
		{KP6,'6'},
		{KP7,'7'},
		{KP8,'8'},
		{KP9,'9'},
		{KPDIV,'/'},
		{KPMUL,'*'},
		{KPMINUS,'-'},
		{KPPLUS,'+'},
		{KPENTER,'\r'},
		{KPDOT,'.'},
        {F1,VK_F1},
        {F2,VK_F2},
        {F3,VK_F3},
        {F4,VK_F4},
        {F5,VK_F5},
        {F6,VK_F6},
        {F7,VK_F7},
        {F8,VK_F8},
        {F9,VK_F9},
        {F10,VK_F10},
        {F11,VK_F11},
        {F12,VK_F12},
 		{0,0}
};

const t_scan2key qwerty_shifted_key[]={
		{0x0e,'~'},
		{0x16,'!'},
		{0x1e,'@'},
		{0x26,'#'},
		{0x25,'$'},
		{0x2e,'%'},
		{0x36,'^'},
		{0x3d,'&'},
		{0x3e,'*'},
		{0x46,'('},
		{0x45,')'},
		{0x4e,'_'},
		{0x55,'+'},
		{0x5d,'|'},
		{0x4c,':'},
		{0x52,'"'},
		{0x41,'<'},
		{0x49,'>'},
		{0x4a,'?'},
        {0x54,'{'},
        {0x5b,'}'},
		{0,0}
};

const t_scan2key qwerty_alt_char[]={
		{0,0}
};

const t_scan2key qwerty_xt_char[]={
		{0x4a,'/'}, // keypad '/'
		{0x5a,'\r'},// keypad ENTER
        {KEY_UP,VK_UP},
        {KEY_DOWN,VK_DOWN},
        {KEY_LEFT,VK_LEFT},
        {KEY_RIGHT,VK_RIGHT},
        {HOME,VK_HOME},
        {END,VK_END},
        {PGUP,VK_PGUP},
        {PGDN,VK_PGDN},
        {INSERT,VK_INSERT},
        {DEL,VK_DELETE},
		{0,0}
};

