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
 * chip8.c
 *
 * Created: 2014-09-26 10:10:45
 *  Author: Jacques Deschênes
 *  Description: machine virtuelle qui exécute du byte code super CHIP
 */ 

// doit-être inclus avant <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Hardware/hardware.h"
#include "pv16vm.h"
#include "text.h"
#include "Hardware/tones.h"
#include "Hardware/joystick.h"
#include "Hardware/systicks.h"
#include "Hardware/spi_ram.h"
#include "Hardware/ps2_kbd.h"
#include "graphics.h"
#include "Hardware/TVout.h"

#define caddr(b1,b2) ((((b1&0xf)<<8)+b2)<<1)
#define rx(b1)  (b1&0xf)
#define ry(b2)  ((b2&0xf0)>>4)

// état de la machine virtuelle
static vm_state_t vms;

#define BIG_SPRITE_SIZE 128
//static uint8_t sprite_buffer[BIG_SPRITE_SIZE];

//bool font_glyph=false;

static uint16_t last_pc;
bool print_vms(const char *msg){
    cursor_t cursor;
    uint8_t key;
    int i;
    
    cursor=get_cursor();
    save_screen(SRAM_SIZE-TV_BUFFER);
    set_cursor(0,0);
	print(msg);
	print("PC:");
	print_hex(last_pc,4);
    print_hex(vms.b1,2);
	print_hex(vms.b2,2);
	new_line();
	print("I:");
	print_hex(vms.ix,4);
	print(" SP:");
	print_hex(vms.sp,2);
	new_line();
	print("VAR[]:\n");
	for (i=0;i<16;i++){
		print_hex(vms.var[i],4);
	}
	print("\n<ESC> quit\n'S' exit trace\nother next step");
	while (!(key=kbd_get_key()));
	if (key=='s') vms.trace=0;
    restore_screen(SRAM_SIZE-TV_BUFFER);
    set_cursor(cursor.col,cursor.line);
    if (key==ESC) return true; else return false;
}



#define MAX_BREAK  (4)

static uint16_t break_points[MAX_BREAK]={0,0,0,0};

void set_break_point(uint16_t addr){
	uint8_t i;
	for (i=0;i<MAX_BREAK;i++)
		if (!break_points[i]) break_points[i]=addr;
}

void clr_break_point(uint16_t addr){
	uint8_t i;
	for (i=0;i<MAX_BREAK;i++){ 
		if ((addr==0xffff) || break_points[i]==addr){ 
			break_points[i]=0;
			break;
		}
	}
}


static uint8_t is_break_point(uint16_t addr){
	uint8_t i;
	for (i=0;i<MAX_BREAK;i++){
		if (break_points[i]==addr){
			return 1;
		}
	}
	return 0;	
}

static bool query_exit(){
    cursor_t cursor;
    char choice;
    
    cursor=get_cursor();
    save_screen(SRAM_SIZE-TV_BUFFER);
    set_cursor(0,0);
    clreol();
    choice=prompt("Exit or Trace(e,t)","et");
    restore_screen(SRAM_SIZE-TV_BUFFER);
    set_cursor(cursor.col,cursor.line);
    if (choice=='e') return true; else return false;
}

// machine virtuelle PV16SOG
uint8_t pv16vm(uint8_t* program, uint8_t flags){
    uint8_t x,y,key=0,pixel_color=cWHITE;
    int16_t n;
    uint16_t code;

    vms.debug=0;
    vms.trace=0;
    if (flags&F_RESET){
        vms.pc=0;
        vms.sp=0;
        vms.ix=0;
    }
    if (flags&F_DEBUG) vms.debug=1;
    if (flags&F_TRACE) vms.trace=1;
    while (1){
        if ((n=kbd_get_key())==ESC){
            if (query_exit()) return VM_BREAK;
            vms.trace=1;
        }else if (n){
            key=n;
        }
        last_pc=vms.pc;
        vms.b1=program[vms.pc++]; 
        vms.b2=program[vms.pc++];
        x=rx(vms.b1);
        y=ry(vms.b2);
        // décodeur d'instruction
        code=(vms.b1&0xf0)<<4;
        switch (code){
        case 0:
            switch(vms.b2&0xf0){
            case 0:
                code=1;
                break;
            case 0x20:
            case 0x30:
            case 0x40:
            case 0x50:
            case 0x60:
            case 0x70:
            case 0x80:
            case 0x90:
            case 0xA0:
                code=vms.b2&0xf0;
                break;
            default:
                code=vms.b2;
                break;
            }//swith
            break;
        case 0xe00:
        case 0xf00:
            code |= vms.b2;
            break;
        case 0x800:
        case 0x900:
            code |= vms.b2&0xf;
            break;
        }
        // exécution de l'instruction
        switch(code){
        case 1: // 0001 | PRT  imprime texte pointé par [I]
            print((const char *)&program[vms.ix]);
            vms.ix+=strlen(&program[vms.ix])+1;
            if (vms.ix&1) vms.ix++;
            break;
        case 0x20: // 002Z | PRN  imprime entier dans VZ
            print_int(vms.var[vms.b2&0xf],0);
            break;
        case 0x30: // 003Z | PRC  imprime caractere dans VZ
            put_char(vms.var[vms.b2&0xf]);
            break;
        case 0x40: // 004Z | KEY VZ  lecture clavier dans VZ
            vms.var[vms.b2&0xf]=key;
            key=0;
            break;
        case 0x50: // 005Z | SCD VZ  glisse affichage vers le bas de VZ pixels
            scroll_down(vms.var[vms.b2&0xf]);
            break;
        case 0x60: // 006Z | SCU VZ  glisse affichage vers le haut de VZ pixels
            scroll_up(vms.var[vms.b2&0xf]);
            break;
        case 0x70: // 007N | SET FG, N  fixe couleur du pixel pour SETP
            pixel_color=vms.b2&0xf;
            break;
        case 0x80: // 008N || SET BG, N fixe la couleur de fond  pour CLS
            set_back_color(vms.b2&0xf);
            break;
        case 0x90: // 009Z | SET FG, N  fixe couleur du pixel pour SETP à partir de VZ
            pixel_color=vms.var[vms.b2&0xf];
            break;
        case 0xA0: // 00AZ || SET BG, N fixe la couleur de fond  pour CLS à partir de VZ
            set_back_color(vms.var[vms.b2&0xf]);
            break;
        case 0xe0: // 00E0, efface l'écran
            cls();
            break;
        case 0xee: // 00EE, sortie de sous-routine
            vms.pc=vms.stack[vms.sp--];
            break;
        case 0xfb: // 00FB, glisse l'affichage vers la droite de 4 pixels
            scroll_right(4);
            break;
        case 0xfc: // 00FC, glisse l'affichage vers la gauche de 4 pixels
            scroll_left(4);
            break;
        case 0xfd:// 00FD,  sortie de l'interpréteur.
            return VM_EXIT_OK;
        case 0xfe: // SAVE | 00FE  sauvegarde affichage dans la SRAM
            save_screen(SRAM_SIZE-TV_BUFFER);
            break;
        case 0xff: // RSTR | 00FF restaure affichage depuis la SRAM
            restore_screen(SRAM_SIZE-TV_BUFFER);
            break;
        case 0x100: // 1NNN saut vers l'adresse NNN
            vms.pc=caddr(vms.b1,vms.b2);
            break;
        case 0x200: // 2NNN  appelle la sous-routine à l'adresse NNN
            vms.stack[++vms.sp]=vms.pc;
            vms.pc=caddr(vms.b1,vms.b2);
            break;
        case 0x300: // 3XKK | SE VX, KK  saute l'instruction suivante si VX == KK
            if (vms.b2>127) n=0xff00+vms.b2; else n=vms.b2;
            if (vms.var[x]==n) vms.pc+=2;
            break;
        case 0x400: // 4XKK | SNE VX, KK Saute l'instruction suivante si VX <> KK
            if (vms.b2>127) n=0xff00+vms.b2; else n=vms.b2;
            if (vms.var[x]!=n)vms.pc+=2;
            break;
        case 0x500: // 5XY0     Saute l'instruction suivante si VX == VY
            if (vms.var[x]==vms.var[y]) vms.pc+=2;
            break;
        case 0x600: // 6XKK | LD VX, KK    VX := KK
            if (vms.b2>127) n=0xff00+vms.b2; else n=vms.b2;
            vms.var[x]=n;
            break;
        case 0x700: // 7XKK  | ADD VX, KK   VX := VX + KK
            if (vms.b2>127) n=0xff00+vms.b2; else n=vms.b2;
            vms.var[x]+=n;
            break;
        case 0x800: // 8XY0  | LD VX, VY   VX := VY
            vms.var[x]=vms.var[y];
            break;
        case 0x801: // 8XY1  | OR VX, VY   VX := VX OR VY
            vms.var[x]|=vms.var[y];
            break;
        case 0x802: // 8XY2  | AND VX, VY   VX := VX AND VY
            vms.var[x]&=vms.var[y];
            break;
        case 0x803: // 8XY3  | XOR VX, VY   VX := VX XOR VY
            vms.var[x]^=vms.var[y];
            break;
        case 0x804: // 8XY4  | ADD VX, VY   VX := VX + VY, VF := carry
            n=(vms.var[x]+vms.var[y])>255;
            vms.var[x]+=vms.var[y];
            vms.var[15]=n;
            break;
        case 0x805: // 8XY5  | SUB VX,VY   VX := VX - VY, VF := not borrow
            n=vms.var[x]>=vms.var[y];
            vms.var[x]-=vms.var[y];
            vms.var[15]=n;
            break;
        case 0x806: // 8XY6  |SHR VX   VX := VX shr 1, VF := carry
            n=(vms.var[x]&1u);
            vms.var[x]>>=1;
            vms.var[15]=n;
            break;
        case 0x807: // 8XY7  | SUBN VX, VY   VX := VY - VX, VF := not borrow
            n=vms.var[y]>=vms.var[x];
            vms.var[x]=vms.var[y]-vms.var[x];
            vms.var[15]=n;
            break;
        case 0x80d: // SCUR VX, VY, positionne curseur texte à VX,VY
            set_cursor(vms.var[x],vms.var[y]);
            break;
        case 0x80e: // 8XYE | SHL VX    VX := VX shl 1, VF := carry
            n=(vms.var[x]&0x8000)>>15;
            vms.var[x]<<=1;
            vms.var[15]=n;
            break;
        case 0x900: // 9XY0 | SNE VX,VY    Saute l'instruction suivante si VX <> VY
            if (vms.var[x]!=vms.var[y]) vms.pc+=2;
            break;
        case 0x901: // 9XY1  TONE , VX, VY  joue une note de la gamme tempérée
            scale_tone(vms.var[x],vms.var[y]<<3,false);
            break;
        case 0x902: //9XY2 | SETP VX,VY  fixe la couleur du pixel à la position VX,VY
            draw_pixel(vms.var[x],vms.var[y],pixel_color);
            break;
        case 0x903: // 9XY3 PIXI VX, VY  inverse le pixel aux coordonnées indiquées par VX,VY
            invert_pixel(vms.var[x],vms.var[y]);
            break;
        case 0x904: // 9X04 NOISE VX bruit blanc, durée indiqué par VX msec.
            noise(vms.var[x]);
            break;
        case 0x905: // 9XY5 TONE VX, VY, WAIT  joue une note de la gamme tempérée attend la fin avant de poursuivre
            scale_tone(vms.var[x],vms.var[y]<<3,true);
            break;
        case 0x906: //9X06  PUSH VX, empile la valeur de VX sur la pile des retours
            vms.stack[++vms.sp]=vms.var[x];
            break;
        case 0x907: //9X07  POP VX, transfert le sommet de la pile dans VX
            vms.var[x]=vms.stack[vms.sp--];
            break;
        case 0x908: // 9X08  SCRX VX,  VX=résolution horizontale
            vms.var[x]=HRES;
            break;
        case 0x909: // 9X09  SCRY VX,  VX=résolution verticale
            vms.var[x]=VRES;
            break;
        case 0x90a: // 9XNA  BSET VX,N  met le bit N de VX à 1  
            vms.var[x]|=(1<<y);
            break;
        case 0x90b: // 9XNB  BCLR VX,N  met le bit N de VX à 0
            vms.var[x]&= ~(1<<y);
            break;
        case 0x90c: // 9XNC  BINV VX,N  inverse le bit N de VX
            vms.var[x]^=(1<<y);
            break;
        case 0x90d: // 9XND  BTSS VX,N  saute l'instruction suivante si bit N==1
            if (vms.var[x]&(1<<y)) vms.pc+=2;
            break;
        case 0x90e: // 9XNE  BTSC VX,N  saute l'nistruction suivante si bi N==0
            if (!(vms.var[x]&(1<<y))) vms.pc+=2;
            break;
        case 0x90f: // 9XYF  GPIX VX,VY  met la couleur du pixel(x,y) dans V0
            vms.var[0]= get_pixel(x,y);
            break;
        case 0xa00: // ANNN   LD I, LABEL   I := NNN
            vms.ix=caddr(vms.b1,vms.b2);  // adressse paire de 13 bits
            break;
        case 0xb00: // BNNN     JP V0, LABEL  saut à 2*(NNN+V0)
            vms.pc=(vms.var[0]<<1)+caddr(vms.b1,vms.b2);
            break;
        case 0xc00: //CXN0 RND N   VX=random_number&(0xffff>>(15-N-1))
            if (y){
                vms.var[x]=random()&(0xffff>>(16-y));
            }else{
                vms.var[x]=random()&0xffff;
            }
            break;
        case 0xd00: //DXYN DRW VX,VY,N dessine un sprite
            n=vms.b2&0xf;
            if (!n){
                    vms.var[15]=draw_sprite((int)vms.var[x],(int)vms.var[y],16,16,(const uint8_t*)&program[vms.ix]);
                }else{
                    vms.var[15]=draw_sprite((int)vms.var[x],(int)vms.var[y],8,n,(const uint8_t*)&program[vms.ix]);
                }
            break;
        case 0xe9e: //EX9E | SKP VX saute l'instruction suivante si le bouton VX est enfoncée
            if (joystick_read()&(vms.var[x])) vms.pc+=2;
            break;
        case 0xea1: //EXA1, SKNP VX saute l'instruction suivante si le bouton VX n'est pas enfoncée
            if (!(joystick_read()&(vms.var[x]))) vms.pc+=2;
            break;
        case 0xf07: // FX07 | LD VX, DT   VX := DT   (minuterie de délais)
            vms.var[x]=game_timer;
            break;
        case 0xf0a: // FX0A | LD VX, K    attend qu'un bouton soit enfoncée et met sa valeur dans VX
            while (!(vms.var[x]=joystick_any_btn()));
            break;
        case 0xf15: // FX15 | LD DT, VX délais en msec
            game_timer=vms.var[x];
            break;
        case 0xf18: // FX18 | BEEP VX   fait entendre un beep d'une durée VX (multiple de 16 msec)
            beep(523,vms.var[x]<<4,1);
            break;
        case 0xf1e: // FX1E  | ADD I, VX    I := I + VX
            vms.ix += vms.var[x]&0xfe;
            break;
        case 0xf55: // FX55 LD [I], VX Sauvegarde les registres V0..VX dans chip_prog à l'adresse M(vms.ix)
            //memcpy(&chip_prog[vms.ix],vms.var,(x+1));
            memmove(&program[vms.ix],(const uint8_t*)&vms.var,x+1);
            break;
        case 0xf65: // FX65 | LD VX, [I] charge les registres V0..VX à partir de la mémoire SRAM à l'adresse M(vms.ix)
            memmove(vms.var,&program[vms.ix],x+1);
            break;
        case 0xf75: // F075 | PUSH I
            vms.stack[++vms.sp]=vms.ix;
            break;
        case 0xf85: // F085 | POP I 
            vms.ix=vms.stack[vms.sp--];
            break;
        default:
                return VM_BAD_OPCODE;
        }//switch
        if (vms.trace || (vms.debug && is_break_point(last_pc))){
                vms.trace=1;
                if (print_vms("Trace print, last executed:\n")) return VM_BREAK;
        }
    }//while(1)
}
