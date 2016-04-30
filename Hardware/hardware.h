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
 * File:   hardware.h
 * Author: jacques
 *
 * Created on 5 juin 2015, 09:35
 */

#ifndef HARDWARE_H
#define	HARDWARE_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#ifndef __asm
    
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>
    
#define _PV16SOG_
#define _FLASH_CONST __attribute__((space(prog)))
#define PSTR const char* __attribute__((space(prog)))

// codes exception
#define EXCP_NONE 0  // pas d'exception
#define EXCP_ADDR 1  // erreur alignement ou adresse non valide
#define EXCP_STACK 2 // débordement de pile
#define EXCP_MATH 3  // exception mathémaitque
#define EXCP_DIV0 4  // division par zéro
#define EXCP_APP 5  // exception générée en logiciel par une une application
#define EXCP_LAST EXCP_APP
#define EXCP_COUNT (EXCP_LAST+1)
    
#define _ISR_NAPSV __attribute__((interrupt,no_auto_psv))

#define _unlock_io()  asm("bclr OSCCON, #6")
#define _lock_io() asm("bset OSCCON, #6")
    
#endif //~ __asm

#define PIC_RAM 0xD000   // PIC24EP512MC202 RAM size
#define POOL_SIZE 20512  // réserve data pool  
// oscillateur primaire avec cristal 8Mhz et PLL 4X
#define XTAL (8) // Mhz
#define FREQ_OSC (140000000) // 140Mhz
#define FCY (FREQ_OSC/2) // 70Mhz
#define AUDIO_CLK (FCY/8)
#define TCY_US 70 // Tcy par microsecondes
#define TCY_MS (FCY/1000) // Tcy par millisecondes

//utilisation des E/S et périphériques
// sorties pixels vidéos PORTB0:3 et PORTB4 NTSC sync
#define VB0     (1<<0)  //RB0
#define VB1     (1<<1)  //RB1
#define VB2     (1<<2)  //RB2
#define VB3     (1<<3)  //RB3
#define SYNC_OUT    (1<<4)  //RB4
#define SYNC_FN   (16)    //numéro de fonction sortie OC1
#define SYNC_TMR  TMR2
#define SYNC_PER  PR2
#define SYNC_RPOR RPOR1    
#define SYNC_PPSbit 0
#define SYNC_RS OC1RS
#define SYNC_R  OC1R
#define SYNC_TCON T2CON
#define SYNC_OCCON1 OC1CON1
#define SYNC_OCCON2 OC2CON2
#define SYNC_IFS  IFS0
#define SYNC_IEC  IEC0
#define SYNC_IF   7   
#define SYNC_IE   7 
#define SYNC_IPC  IPC1
#define SYNC_IPbit 12    
#define VIDEO_OCCON1 OC2CON1
#define VIDEO_OCCON2 OC2CON2
#define VIDEO_R  OC2R
#define VIDEO_RS OC2RS    
#define VIDEO_IPC IPC1   
#define VIDEO_IPbit 8    
    
    // atari 2600 joystick
// joystick switches
#define UP      (1<<15) //RB15
#define DOWN    (1<<14) //RB14
#define LEFT    (1<<13) //RB13
#define RIGHT   (1<<12) //RB12
#define FIRE    (1<<11) //RB11
// sortie audio PORTB:5
// utilise  OC3
#define AUDIO_OUT 5
#define AUDIO   (1<<AUDIO_OUT)  //RB5
#define AUDIO_FN   (18<<8)  // numéro de fonction sortie OC3
#define AUDIO_FN_MSK (31<<8)
#define AUDIO_TCON  T4CON // audio utilise TIMER4
#define AUDIO_CLK_PS (2<<4)
#define AUDIO_RPOR RPOR1
#define AUDIO_TRIS TRISB
#define AUDIO_PORT PORTB
#define AUDIO_LAT LATB
#define AUDIO_PER PR4
#define AUDIO_RS OC3RS
#define AUDIO_R  OC3R
#define AUDIO_OCCON1 OC3CON1
#define AUDIO_OCCON2 OC3CON2
#define AUDIO_TSEL (2<<10)
#define TOGGLE_MODE  3    

    
// SPIxSTAT bits
#define BIT_SPIEN (15)      //enable bit
#define BIT_SPISIDL (13)    // stop when MCU in IDLE
#define BIT2_SPIBEC (10)    // 10:8bytes in buffer (enhanced mode)
#define BIT1_SPIBEC (9)
#define BIT0_SPIBEC (8)
#define BIT_SRMPT  (7)      // tx shift register empty  (enhanced mode)
#define BIT_SPIOVR (6)      // rx buffer overflow
#define BIT_SRXMPT (5)      // receive FIFO empty (enhanced mode)
#define BIT2_SISEL (4)      // 4:2 interrupt mode (enhanced mode)
#define BIT1_SISEL (3)
#define BIT0_SISEL (2)
#define BIT_SPITBF (1)      // transmit buffer full
#define BIT_SPIRBF (0)      // receive buffer full
// SPIxCON1 bits
#define BIT_DISSCK (12)     // disable serial clock pin
#define BIT_DISSDO (11)     // disable SDO pin
#define BIT_MODE16 (10)     // mode 16 bits
#define BIT_SMP    (9)      // input sample phase
#define BIT_CKE    (8)      // clock edge
#define BIT_SSEN   (7)      // slave select (slave mode)
#define BIT_CKP    (6)      // clock polarity
#define BIT_MSTEN  (5)      // master mode enable
#define BIT2_SPRE  (4)      // 4:2 secondary clock prescale
#define BIT1_SPRE  (3)
#define BIT0_SPRE  (2)
#define BIT1_PPRE  (1)      // 1:0 premary clock prescale
#define BIT0_PPRE  (0)
// SPIxCON2 bits
#define BIT_FRMEN  (15)     // framed mode enable
#define BIT_SPIFSD (14)     // frame sync direction
#define BIT_SPIFPOL (13)    // frame sync polarity
#define BIT_SPIPE   (1)     // sync pulse edge select
#define BIT_SPIBEN  (0)     // enhanced mode enable
// resources carte SD et SRAM
#define SPI_PORT PORTB
#define SPI_LAT  LATB
#define SPI_TRIS TRISB
// broches MCU utilisées par SDC et SRAM sur PORTB
#define BIT_SRAM_SEL 6
#define BIT_SDC_SEL  10
#define BIT_SPI_CLK  7
#define BIT_SPI_MOSI 8
#define BIT_SPI_MISO 9
// SPI
#define SPI_TX_BUF SPI1BUF
#define SPI_RX_BUF SPI1BUF
#define SPI_STAT SPI1STAT
#define SPI_CON1 SPI1CON1
#define SPI_CON2 SPI1CON2
//#define _set_spi_mosi()  {RPOR3 &= ~0x1f;RPOR3 |= MOSI_FN;}
//#define _set_spi_clock() {RPOR4 &= ~0x1f;RPOR4 |= SCLK_FN;}
//#define _set_spi_miso()  {RPINR22 &= 0xffe0; RPINR22|=BIT_SPI_MISO;}
// SPI RAM SFR
#define SRAM_TRIS TRISB
#define SRAM_LAT  LATB
#define SRAM_PORT PORTB
#define SRAM_SS_RP BIT_SRAM_SEL
//  carte SD 
#define SDC_TRIS        TRISB
#define SDC_LAT         LATB
#define SDC_PORT        PORTB
#define SDC_SS_RP       BIT_SDC_SEL
#define SDC_CD_TRIS     TRISA
#define SDC_CD_PORT     PORTA
#define SDC_CD_RP       (4)  // Card detect sur RA:4

#ifndef __asm
// select/déselect RAM SPI
#define _sram_select()  LATBbits.LATB6=0
#define _sram_unselect() LATBbits.LATB6=1
// select/déselect carte SD
#define select_card()  LATBbits.LATB10=0
#define unselect_card() LATBbits.LATB10=1
//détection carte SD sur RA4
#define _sdcard_present()  (SDC_CD_PORT & (1<<SDC_CD_RP))
#endif //__asm
    
// joystick
#define JOYPORT PORTB
#define JOYLAT  LATB
#define JOYTRIS TRISB
// PORTB bits
#define BTN_UP      (16)
#define BTN_DOWN    (8)
#define BTN_LEFT    (4)
#define BTN_RIGHT   (2)
#define BTN_FIRE    (1)

// PS/2 keyboard
#define KBD_PORT    PORTA
#define KBD_LAT     LATA
#define KBD_TRIS    TRISA
#define KBD_CLK     (0)     //PORTA bit 0
#define KBD_DAT     (1)     //PORTA bit 1
#define KBD_CN      (0)     
#define KBD_IEC     IEC1
#define KBD_IFS     IFS1
#define KBD_INTBIT  (3)
#define KBD_IPC     IPC4
#define KBD_IPCBIT  (12)
#define KBD_TIMER   TMR4
#define KBD_TCON    T4CON
#define KBD_TMR_PER PR4
    
// si hardware.h est inclus
// dans un fichier *.S
// ces prototypes ne doivent pas
// être visible. Utilisation:
//#define __asm
//#include "hardware.h"
//#undef __asm
#ifndef __asm 
//initialisation matériel.    
void hardware_init(void);
//void *malloc_fail(uint16_t size);
//alloue de la RAM dans le pool
void *dalloc(uint16_t size);
//libère la RAM à partir de la position ptr
void dfree(void *ptr);
//retourne la position libre
void* dmark();
// retourne le nombre d'octets libres
uint16_t dfree_size();
// utilisé par application pour générer un reset
void software_exception(uint8_t code);
// initialise le PRNG
void randomize();
// ajuste la position du cadre vidéo
extern void video_adjust();

#endif //__asm

#ifdef	__cplusplus
}
#endif

#endif	/* HARDWARE_H */

