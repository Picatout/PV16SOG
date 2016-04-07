/*
* Copyright 2015, Jacques Deschênes
* This file is part of PICVisionPortable.
*
*     PICVisionPortable is free software: you can redistribute it and/or modify
*     it under the terms of the GNU General Public License as published by
*     the Free Software Foundation, either version 3 of the License, or
*     (at your option) any later version.
*
*     PICVisionPortable is distributed in the hope that it will be useful,
*     but WITHOUT ANY WARRANTY; without even the implied warranty of
*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*     GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public License
*     along with PICVisionPortable.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <xc.h>

#include "HardwareProfile.h"

volatile uint32_t systick; // millisecond ticks
volatile uint16_t game_timer;

#if SDC_SUPPORT
#include "../FAT/sd_raw.h"
bool sdcard_available=false;
#endif

void HardwareInit(){
// clock setting
    CLKDIV=0;
// systick timer configuration
    SYSTICK_IPC &= ~(7<<12); //remet niveau à zéro T1IPL
    SYSTICK_IPC |= 1<<12;  // met niveau à 1  T1IPL
    SYSTICK_IF=0;
    SYSTICK_IE=1;
    systick=0;
    PR1=FCY/1000;
    T1CON=0xA000;
// tone init
    TONE_TRIS &= ~(TONE1_OUT|TONE2_OUT);
    TONE1_PIN_PPS=TONE1_OUT_FN; // select OCx on audio_out pin
    TONE1_OCCON=(1<<13);
    TONE1_TMRCON = (1<<13);
    TONE1_IPC=3;
    TONE2_PIN_PPS=TONE2_OUT_FN;
    TONE2_OCCON=(1<<13)|(1<<3);
    TONE2_TMRCON=(1<<13);

// LCD port configuraton
    LCD_LAT &= ~(LCD_PINS|LCD_RST); // 0 volt on all LCD pins
    LCD_ODC |= LCD_PINS|LCD_RST;  // LCD pins configured Open Drain.
    LCD_TRIS &= ~(LCD_PINS|LCD_RST); // LCD pins all outputs.

// Keypad input pins
    TRISA = KP_MASK;
// no analog input
    AD1PCFG=0xffff;

#if SDC_SUPPORT
    sdcard_available=sd_raw_init();
#endif

    // lock PPS configuration
    asm("mov #0x46, W0\n"
        "mov W0, OSCCON\n"
        "mov #0x57, W0\n"
        "mov  W0, OSCCON\n"
        "bset OSCCON, #6\n");

}//f()

#include "../tone.h"

void pause(uint16_t msec){
    uint32_t t0;
    t0=systick+msec;
    while (systick<t0);
}//f()

void __attribute__((interrupt,no_auto_psv)) _T1Interrupt(void){
    systick++;
    if (tone_timer[1]){
        tone_timer[1]--;
        if (!tone_timer[1]){
            tone1_off();
        }
    }
    if (tone_timer[2]){
        tone_timer[2]--;
        if (!tone_timer[2]){
            tone2_off();
        }
    }
    if (game_timer){
        game_timer--;
    }
    SYSTICK_IF=0;
}//f()
