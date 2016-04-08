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
 *  ps2_kbd.c
 *  ref: http://www.computer-engineering.org/
 */

#include <stdint.h>

#include "systicks.h"
#include "ps2_kbd.h"
#include "QWERTY.h"
#include "TVout.h"
#include "tones.h"
#include "../stackvm.h"

#define BREAK_CODE 0xF0
#define XTD_CODE  0xE0
// réponses du clavier aux commandes
#define KBD_ACK   0xFA  // ACK confirmation envoyé par le clavier
#define KBD_RSND  0xFE  // le clavier demande de renvoyer la commande
#define BAT_OK    0xAA  // test clavier réussi (Basic Assurance Test)
#define BAT_ERROR 0xFC  // échec test clavier
// indicateurs dans kbd_flags
#define F_ERRPAR (1<<0) // erreur paritée dans rx_flags
#define F_TX   (1<<1) // transmission en cours
#define F_ERRTX (1<<2) // erreur de transmissiopn
#define F_ACK  (1<<3)  // reçu KBD_ACK
#define F_RSND (1<<4)  // reçu KBD_RSND
#define F_XTDKEY (1<<5) // reçu code étendu
#define F_KEYREL (1<<6) // reçu code relâchement
#define F_NOPS2 (1<<7) // clavier absent ou défectueux
// lumières clavier
#define F_LED_SCROLL 1 // bit indicateur scroll_lock dans kbd_leds
#define F_LED_NUM  2 // bit indicateur numlock dans kbd_leds
#define F_LED_CAPS 4 // bit indicateur capslock dans kbd_leds
#define F_LED_CHANGE 128 // indicateur de changement d'état

#define QUEUE_SIZE (16) // doit-être une puissance de 2
#define PS2_QUEUE_SIZE (16)

volatile static uint8_t  kbd_queue[QUEUE_SIZE];
// les variables suivantes sont utilisées par ps2_int.S
volatile uint16_t ps2_queue[PS2_QUEUE_SIZE];
volatile uint8_t __attribute__((near))ps2_head=0;
volatile uint8_t __attribute__((near))ps2_tail=0;
volatile uint16_t __attribute__((near))ps2_shiftout=0; // registre à décalage transmission
volatile uint16_t __attribute__((near))kbd_flags=F_NOPS2;


volatile static uint8_t head=0, tail=0;
volatile uint8_t __attribute__((near))bitcntr=0;

volatile static uint16_t kbd_leds;
volatile uint8_t key_state; // état des touches d'alteration: shift, ctrl ,alt

bool kbd_ready(){
    return (kbd_flags&F_NOPS2)==0;
}

bool kbd_reset(){
    key_state=0;
    kbd_flags |= F_NOPS2;
    kbd_send(KBD_RESET);
    pause_timer=750;
    while(pause_timer){
        if (!(kbd_flags & F_NOPS2)){
            tail=0;
            head=0;
            pause_timer=1;
            return true;
        }
    }
    return false;
}//f()

int kbd_queue_empty(){
    return head==tail;
}//f()

uint8_t search_table(const t_scan2key table[], uint8_t code){
    int i=0;
    while (table[i].ascii && table[i].code){
        if (table[i].code==code) return table[i].ascii;
        i++;
    }
    return 0;
}//f())

void kbd_update_leds(){
    if (kbd_leds&F_LED_CHANGE){
        kbd_send(KBD_LED);
        if (kbd_flags & F_ACK){
            kbd_leds &=7;
            kbd_send(kbd_leds);
        }
    }
}//f()

uint8_t kbd_get_key(){
    uint8_t code,key;
    
    kbd_update_leds();
    if (head==tail) return 0;
    code = kbd_queue[head++];
    head &= QUEUE_SIZE-1;
    if (code==XTD_CODE){
        while (tail==head);
        code = kbd_queue[head++];
        head &= QUEUE_SIZE-1;
        key=search_table(qwerty_xt_char,code);
    }else{
        if (key_state&F_SHIFT){
            key=search_table(qwerty_shifted_key,code);
            if (!key){
                key=search_table(qwerty,code);
            }
        }else{
            key=search_table(qwerty,code);
        }
    }
    if ((key>='a') && (key<='z')){
        if (key_state & F_CAPS)
            key-=32;
        if (key_state & F_SHIFT)
            key ^= 0x20;
    }
    return key;
}//f()

uint8_t kbd_wait_key(){
    uint8_t key;
    while (!(key=kbd_get_key()));
    return key;
}//f()

void kbd_send(uint8_t code){
    int i,parity;
#define STOP_BIT (1<<9)
#define PARITY_BIT (1<<8)    
    parity=0;
    for (i=0;i<8;i++) if (code&(1<<i)) parity++;
    ps2_shiftout = STOP_BIT|code;
    if (!(parity&1)) ps2_shiftout |= PARITY_BIT;
    KBD_IEC &= ~(1<<KBD_INTBIT); // désactive interruption _CNInterrupt
    KBD_LAT &= ~(1<<KBD_CLK);
    KBD_TRIS &= ~(1<<KBD_CLK); // ligne clock en sortie
    kbd_flags |= F_TX;
    kbd_flags &= ~(F_ERRTX|F_ACK|F_RSND);
//    bitcntr=0;
    asm("repeat #%0\nnop\n"::"i"(150*TCY_US));//delais >=150µsec
    KBD_LAT &= ~(1<<KBD_DAT); //  request to send
    KBD_TRIS &= ~(1<<KBD_DAT); // ligne data en sortie
    KBD_IFS &= ~(1<<KBD_INTBIT); 
    KBD_IEC |= (1<<KBD_INTBIT); // réactive interruption
    KBD_TRIS |= (1<<KBD_CLK); // libération clock
    pause_timer=50;
    while (pause_timer && (kbd_flags & F_TX));
    if (!pause_timer){
        kbd_flags |= F_NOPS2;
        return;
    }else{
        while (!(kbd_flags&(F_ACK|F_RSND)));
        while (!(KBD_PORT & (1<<KBD_CLK)));
    }
}//f()



#define SENTRY 0x400

_ISR_NAPSV void _CNInterrupt(void){
    static uint16_t ps2_shiftin=SENTRY; // registre à décalage réception
    
    KBD_IFS &= ~(1<<KBD_INTBIT);
    if (KBD_PORT & (1<<KBD_CLK)) return;
    if (kbd_flags & F_TX){ // transmission (MCU -> clavier)
        switch(bitcntr){
            case 9: // stop bit, relâchement de la ligne data
                KBD_TRIS |= (1<<KBD_DAT);
                break;
            case 10: // test ack bit.
                if (KBD_PORT & (1<<KBD_DAT)){
                    kbd_flags |= F_ERRTX;
                }else{
                    kbd_flags &= ~F_ERRTX;
                }
                kbd_flags &= ~F_TX; // fin transmission
                bitcntr=-1;
                ps2_shiftin=SENTRY;
                break;
            default: // 0-8
                if (ps2_shiftout&1){
                    KBD_LAT |= (1<<KBD_DAT);
                }else{
                    KBD_LAT &= ~(1<<KBD_DAT);
                }
                ps2_shiftout >>=1;
        }//switch
        bitcntr++;
    }else{ // réception caractère (clavier -> MCU)
        if (KBD_PORT & (1<<KBD_DAT)) ps2_shiftin |=0x8000;
        if (ps2_shiftin&1){
            //b5 start bit==0, b15 stop bit==1
            if ((ps2_shiftin&0x8020)==0x8000){
                // accepté: aligne sur bit 0 et élimine 
                // start et stop bits, sauvegarde
                ps2_queue[ps2_tail++]=(ps2_shiftin>>6)&0x1ff;
                ps2_tail &= PS2_QUEUE_SIZE-1;
                IFS1bits.T4IF =0;
                IEC1bits.T4IE =1;
            }
            ps2_shiftin=SENTRY;
        }else{
            ps2_shiftin >>= 1;
        }
    }//else
}//f()


_ISR_NAPSV  void _T4Interrupt(){
    uint16_t code;
    uint8_t parity;
    int i;
    
    IFS1bits.T4IF =0;
    if (ps2_head==ps2_tail){
        IEC1bits.T4IE =0;
    }else{
        code = ps2_queue[ps2_head++];
        ps2_head &= PS2_QUEUE_SIZE-1;    
        parity=0;
        for (i=0;i<9;i++){
            if (code&(1<<i)) parity++;
        }
        if ((parity&1)==1){
            code&=0xff;
            switch(code){
                case BAT_OK:
                    kbd_flags &= ~F_NOPS2;
                    break;
                case BAT_ERROR:
                    kbd_flags |= F_NOPS2;
                    break;
                case KBD_ACK:
                    kbd_flags |= F_ACK;
                    break;
                case KBD_RSND:
                    kbd_flags |= F_RSND;
                    break;
                case BREAK_CODE:
                    kbd_flags |= F_KEYREL;
                    break;
                case XTD_CODE:
                    kbd_flags |= F_XTDKEY;
                    break;
                case CAPS_LOCK:
                    if (kbd_flags&F_KEYREL)
                        kbd_flags &= ~F_KEYREL;
                    else{
                        key_state ^= F_CAPS;
                        kbd_leds |= F_LED_CHANGE;
                        kbd_leds ^=F_LED_CAPS;
                    }
                    break;
                case NUM_LOCK:
                    if (kbd_flags&F_KEYREL)
                        kbd_flags &= ~F_KEYREL;
                    else{
                        kbd_flags ^= F_NUM;
                        kbd_leds |= F_LED_CHANGE;
                        kbd_leds ^=F_LED_NUM;
                    }
                    break;
                case SCROLL_LOCK:
                    kbd_flags &= ~F_KEYREL;
                    break;
                case LSHIFT:
                    if (kbd_flags&F_KEYREL){
                        key_state&=~F_LSHIFT;
                    }else{
                        key_state|=F_LSHIFT;
                    }
                    kbd_flags&=~(F_KEYREL+F_XTDKEY);
                    break;
                case RSHIFT:
                    if ((kbd_flags&F_KEYREL)){
                        key_state&=~F_RSHIFT;
                    }else{
                        key_state|=F_RSHIFT;
                    }
                    kbd_flags &= ~(F_XTDKEY+F_KEYREL);
                    break;
                case CTRL:
                    if (kbd_flags & F_XTDKEY){
                        if (kbd_flags & F_KEYREL){
                            key_state &= ~F_RCTRL;
                        }else{
                            key_state |= F_RCTRL;
                        }
                    }else{
                        if (kbd_flags & F_KEYREL){
                            key_state &= ~F_LCTRL;
                        }else{
                            key_state |= F_LCTRL;
                        }
                    }
                    kbd_flags &= ~(F_KEYREL+F_XTDKEY);
                    break;
                case ALT:
                    if (kbd_flags & F_XTDKEY){
                        if ((kbd_flags & F_KEYREL)){
                            key_state &= ~F_ALTCHAR;
                        }else{
                            key_state |= F_ALTCHAR;
                        }
                    }else{
                        if ((kbd_flags & F_KEYREL)){
                            key_state &= ~F_LALT;
                        }else{
                            key_state |= F_LALT;
                        }
                    }
                    kbd_flags &= ~(F_KEYREL+F_XTDKEY);
                    break;
                case DEL:
                    if ((key_state & (F_LALT|F_LCTRL))==(F_LALT|F_LCTRL)){
                        asm("reset");
                    }
                default:
                    if (!(kbd_flags&F_KEYREL)){
                        if (kbd_flags &F_XTDKEY){
                            kbd_queue[tail++]=XTD_CODE;
                            tail &= QUEUE_SIZE-1;
                            kbd_flags &= ~F_XTDKEY;
                        }
                        kbd_queue[tail++]=code;
                        tail &= QUEUE_SIZE-1;
                    }else{
                        kbd_flags &= ~(F_KEYREL+F_XTDKEY);
                    }
                    break;

            }//switch
        }
    }
} 

