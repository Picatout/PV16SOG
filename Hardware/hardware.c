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
 *  initialisation du MCU
 */
#include <stdlib.h>
#include <string.h>
#include "hardware.h"
#include "systicks.h"
#include "spi_ram.h"
#include "../text.h"
#include "../errors.h"
#include "ps2_kbd.h"
#include "../graphics.h"
#include "tones.h"
#include "../stackvm.h"

//volatile unsigned msec_ticks; // compteur de millisecondes
//volatile unsigned long __attribute__ ((address(0x1006))) lfsr=0xace1;  // registre du générateur pseudo-hasard.




//routines de gestion mémoire fonctionnant comme une pile
//le dernier bloc alloué doit-être le premier libéré.
extern uint8_t data_pool[POOL_SIZE];
static void *here=data_pool;
#define ALLOC_END  (void*)(data_pool+POOL_SIZE)    

void *dalloc(uint16_t size){
    void *new_ptr;
    
    if (size&1) size++; // doit-être pair pour conserver alignement.
    new_ptr=here;
    if ((ALLOC_END-size)<here) return NULL;
    here+=size;
    return new_ptr;
}//f()

void dfree(void *ptr){
    memset(ptr,0,here-ptr);
    here=ptr;
    if (((uint16_t)here) & 1){
        *(uint8_t*)here++=0;
    }
}//f()

uint16_t dfree_size(){
    return (uint16_t)(ALLOC_END-here);
}//f()

void* dmark(){
    return here;
}//f();

//initialisation core et minuterie systicks
//utilise TIMER1
void systicks_init(void){
    CLKDIV = 0; // PLLPRE=0->N1=2, PLLPOST=2->N2=2
    PLLFBD = 68; // M=35
    INTCON1bits.NSTDIS=0; // NSTDIS interruptions multiniveaux
    msec_ticks=0;
    pause_timer=0;
    PR1=TCY_MS/8-1;  // période TIMER1 = 1 millisecondes
    IPC0bits.T1IP=3; // priorité d'interruption 3
    T1CON |= (1<<15)+(1<<4); // TON, activation du TIMER1
    IFS0bits.T1IF = 0;  // T1IF, raz indicateur d'interuption TMR1
    IEC0bits.T1IE = 1;   // T1IE, activation interrupton
}//f()

#include "ntsc_const.h"

uint16_t video_delay=VIDEO_DELAY;

//initialisation des périphériques
// utilisé par le générateur NTSC
void TVout_init(void){
    // PORTB B0:B3 DAC vidéo, BIT4 NTSC sync
    TRISB &= ~0x1f; // PORTBB0:4 sorties
    LATB = 0; // sortie DAC=0
    // configuration PPS pour la sync. vidéo
    SYNC_RPOR = SYNC_FN; 
    SYNC_PER=HLINE; // période ligne horizontale 63,5µS
    SYNC_TCON|=(1<<15); // TON, activation TMR2
    //configuration SYNC output compare
    SYNC_RS=HLINE;
    SYNC_R=HSYNC;
    SYNC_OCCON1=5; // mode PWM
    // configuration VIDEO output compare
    VIDEO_R=HLINE;
    VIDEO_RS=VIDEO_DELAY;
    VIDEO_OCCON1=5; // mode PWM
    // configurtation interruption SYNC et VIDEO
    // priorité d'interruption 5 pour TIMER2 
    SYNC_IPC &= ~(7<<SYNC_IPbit);
    SYNC_IPC |= (6<<SYNC_IPbit);
    //priorité interruption vidéo
    VIDEO_IPC &= ~(7<<VIDEO_IPbit);
    VIDEO_IPC |= (6<<VIDEO_IPbit);
    // activation interruption TIMER2
    SYNC_IFS &= ~(1<<SYNC_IF);
    SYNC_IEC |= (1<<SYNC_IE);
}//f()

// le même SPI est utilisé pour
// la SRAM et la carte SD
void spi_init(){
    SPI_TRIS &= ~((1<<BIT_SRAM_SEL)|(1<<BIT_SDC_SEL)|(1<<BIT_SPI_MOSI)|(1<<BIT_SPI_CLK));
    SPI_LAT |= ((1<<BIT_SRAM_SEL)|(1<<BIT_SDC_SEL));
    //_set_spi_clock();
    //_set_spi_mosi();
    //_set_spi_miso();
    SPI_CON1=(1<<BIT_MSTEN)|(5<<BIT0_SPRE)|(3<<BIT0_PPRE)|(1<<BIT_CKE); // master mode, clk=Fcy/2
    SPI_CON2=0; //1<<BIT_SPIBEN;//enhanced mode
    SPI_STAT=(1<<BIT_SPIEN);
    SDC_TRIS &= ~(1<<SDC_SS_RP); // carte SD select
    SRAM_TRIS &= ~(1<<SRAM_SS_RP); // SPI RAM select
    _sram_unselect();
    unselect_card();
    sram_set_mode(MODE_SEQ);
}//f()

void kbd_init(){
    KBD_TRIS |= (1<<KBD_CLK)+(1<<KBD_DAT);
    KBD_LAT |= (1<<KBD_CLK)+(1<<KBD_DAT);
    CNPUA = 0;
    CNPDA = 0;
    ODCA |= (1<<KBD_CLK)+(1<<KBD_DAT);
    CNENA |= (1<<KBD_CN);
    KBD_IPC |= (7<<KBD_IPCBIT); // interruption priorité 7
    KBD_IFS &= ~(1<<KBD_INTBIT);
    KBD_IEC |= (1<<KBD_INTBIT);
    // utilisation TMR4 pour compléter 
    //traitement ps2_queue vers kbd_queue
    IPC6bits.T4IP=2;
    // TCKPS 1:8
    KBD_TCON|=(1<<15)+(1<<4);
    // période 1,5msec. 
    KBD_TMR_PER=TCY_MS/8+TCY_MS/16;
    
}//f();

void audio_init(){
    tone_len=0;
    AUDIO_TRIS &= ~AUDIO;
    AUDIO_RPOR &= ~AUDIO_FN_MSK;
    AUDIO_RPOR |= AUDIO_FN;
    AUDIO_OCCON1 = 0;
    AUDIO_OCCON1 |= AUDIO_TSEL;
    AUDIO_OCCON2 = 0;
    AUDIO_OCCON2 |= 0xE|(1<<7); //
}//f()

//REF: https://en.wikipedia.org/wiki/Xorshift
//REF: http://excamera.com/sphinx/article-xorshift.html
//xorshift32 pseudo random number generator
uint32_t seed = 7;  // 100% random seed value
uint32_t random()
{
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;
  return seed;
}

void randomize(){
    seed=msec_ticks|1;
}

//pour réduire la consommation on désactive les périphérique 
// inutilisés, 1->désactive le périphérique
static void disable_unused_peripherals(){
    ANSELA=0; // pas d'entrées analogique
    PMD1 = 0xA6F1;
    PMD2 = 0x0008;
    PMD3 = 0x0482;
    PMD4 = 0x000C;
    PMD7 = 0x0018;
}

__eds__ static PSTR EXCP_MSG[EXCP_COUNT]={
    "rebooted on ",
    "address exception\n",
    "stack exception\n",
    "math exception\n",
    "division by 0\n",
    "resetted by application\n"
};

void hardware_init(void){
    char msg[64];
    
    _unlock_io();
    RCON &= ~(1<<5); //désactivation WDT
    DSRPAG=1;
    DSWPAG=1;
    CORCONbits.SFA = 1; // W14 et W15 ignore DSRPAG,DSWPAG
    disable_unused_peripherals();
    systicks_init();
    TVout_init();
    spi_init();
    audio_init();
    kbd_init();
    // verrouille la configuration des E/S
    _lock_io();
    uint8_t byte;
    uint32_t pc,pch;
    byte=sram_read_byte(SRAM_SIZE-1);
    pc=sram_read_word(SRAM_SIZE-3);
    pch=sram_read_byte(SRAM_SIZE-4);
    sram_clear_block(SRAM_SIZE-4,4);
    pc=(pch<<16)|pc;
    if ((byte>EXCP_NONE) && (byte<EXCP_COUNT)){
        pstrcpy(msg,EXCP_MSG[byte]);
        print(msg);
        print("\nPC: ");
        print_hex(pc,6);
        prompt("\nany key...","*");
    }
}//f()

extern uint16_t top_line;
// ajustement de la position d'écran
void video_adjust(){
    uint8_t key=0;
    uint16_t old_top=0;
    uint16_t old_hdly=0;
   do{
        
        if ((old_top!=top_line) || (old_hdly!=video_delay)){ 
            old_top=top_line;
            old_hdly=video_delay;
            cls(bg_color);
            set_cursor(1,2);
            print("use arrow keys to position raster.\n");
            print_int(old_top,3); 
            put_char(',');
            print_int(old_hdly,3);
            draw_rect(0,0,HRES-1,VRES-1,cWHITE);
            gray_scale(VRES-20,20);
        }
        pause_timer=20;
        while (pause_timer);
        key=kbd_get_key();
        switch(key){
            case VK_UP:
                if (top_line>MINTOP) top_line--;
                break;
            case VK_DOWN:
                if (top_line<MAXTOP) top_line++;
                break;
            case VK_LEFT:
                if (video_delay>ADJMIN){
                    video_delay-=6;
                    VIDEO_RS=video_delay;
                }
                break;
            case VK_RIGHT:
                if (video_delay<ADJMAX){
                    video_delay+=6;
                    VIDEO_RS=video_delay;
                }
                break;
            default:
                break;
        }
    }while (key!=ESC);
    cls(bg_color);
}


#include "TVout.h"
#define get_exception_address(pc,pch) asm("mov [W15-24],W0\nMOV W0,%0\nmov [W15-22],W0\nmov W0,%1":"=g"(pc),"=g"(pch))

_ISR_NAPSV void _AddressError(void){
    uint16_t pc,pch;
    
    get_exception_address(pc,pch);
    pc&=0xfffe;
    pch&=0x3f;
    sram_write_word(SRAM_SIZE-3,pc);
    sram_write_byte(SRAM_SIZE-4,pch&255);
    sram_write_byte(SRAM_SIZE-1,EXCP_ADDR);
    asm("reset");
}//f()

_ISR_NAPSV void _MathError(void){
    uint16_t pc,pch;
    
    get_exception_address(pc,pch);
    pc&=0xfffe;
    pch&=0x3f;
    sram_write_word(SRAM_SIZE-3,pc);
    sram_write_byte(SRAM_SIZE-4,pch&255);
    if (INTCON1bits.DIV0ERR){
        sram_write_byte(SRAM_SIZE-1,EXCP_DIV0);
        
    }else{
        sram_write_byte(SRAM_SIZE-1,EXCP_MATH);
    }
    asm("reset");
}//f()


_ISR_NAPSV void _StackError(void){
    uint16_t pc,pch;
    get_exception_address(pc,pch);
    pc&=0xfffe;
    pch&=0x3f;
    sram_write_word(SRAM_SIZE-3,pc);
    sram_write_byte(SRAM_SIZE-4,pch&255);
    sram_write_byte(SRAM_SIZE-1,EXCP_STACK);
    asm("reset");
}//f()


