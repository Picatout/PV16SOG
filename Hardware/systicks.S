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
   compteurs système incrémenté à chaque milliseconde
   boucle à toute les 65,536 secondes.
*/

.include "p24EP512MC202.inc"
#define __asm
#include "hardware.h"
#undef __asm
    
;.equiv TMR_ON, 15
;.equiv AUDIO_TMR, TMR3
;.equiv AUDIO_OCCON1, OC3CON1
.equ STACK_LIMIT, 0x8000
.equ STACK_SIZE, 0x400
.equ STACK_BASE,  STACK_LIMIT-STACK_SIZE-16
.section .nbss, bss, near

.section .ndata data, near 
.global _msec_ticks
_msec_ticks: .space 2

 ; position de la pile et grandeur
 .section user_stack, stack, address(STACK_BASE)
 .space STACK_SIZE+16
 
 .global _data_pool
 .section .dpool, data, address(STACK_BASE-POOL_SIZE)
 _data_pool:
 .space POOL_SIZE
 
.text


;********************
; interruption TIMER1
;********************
.global __T1Interrupt
__T1Interrupt:
    push W0
    inc _msec_ticks
    cp0 _pause_timer
    bra z, 1f
    dec _pause_timer
1:
    cp0 _tone_len
    bra z, 2f
    dec _tone_len
    bra nz, 2f
    mov #0xFFF8,W0
    and AUDIO_OCCON1
2:
    bclr    IFS0, #T1IF
    pop W0
    retfie
    
.end