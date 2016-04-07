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


#define __asm
#include "hardware.h"
#include "../stackvm.h"
#include "tones.h"
#undef __asm

.section .ndata data, near
.global _tone_len
_tone_len: .space 2
    
.text
.global _vm_beep, _tone    
_scale:
    .word 69; DO2#
    .word 73; RE2
    .word 78; RE2#
    .word 82; MI2
    .word 87; FA2
    .word 92; FA2#
    .word 98; SOL2
    .word 104; SOL2#
    .word 110; LA2
    .word 117; LA2#
    .word 123; SI2
    .word 131; DO3
    .word 139; DO3#
    .word 147; RE3
    .word 156; RE3#
    .word 165; MI3
    .word 175; FA3
    .word 185; F3#
    .word 196; SOL3
    .word 208; SOL3#
    .word 220; LA3
    .word 233; LA3#
    .word 247; SI3
    .word 262; DO4
    .word 277; DO4#
    .word 294; RE4
    .word 311; RE4#
    .word 330; MI4
    .word 349; FA4
    .word 370; FA4#
    .word 392; SOL4
    .word 415;  SOL4#
    .word 440; LA4
    .word 466; LA4#
    .word 494; SI4
    .word 523; DO5
    .word 554; DO5#
    .word 587; RE5
    .word 622; RE5#
    .word 659; MI5
    .word 698; FA5
    .word 740; FA5#
    .word 784; SOL5
    .word 831; SOL5#
    .word 880; LA5
    .word 932; LA5#
    .word 988; SI5
    .word 1046; DO6


_tone: // ( n1 n2 n3 --  ) n1->note, n2->durée, n3->wait
    push DSRPAG
    mov [dsp-4],W0
    and #0x3f,W0
    cp W0,#SCALE_SIZE
    bra lt, 1f
    pop DSRPAG
    return
1:
    sl W0,W0
    mov #edspage(_scale),W1
    mov W1,DSRPAG
    mov #edsoffset(_scale),W1
    mov [W1+W0],W0
    mov W0,[dsp-4]
    pop DSRPAG

.extern _tone_len
.global _beep    
// génère un beep sonore
_vm_beep: // ( freq len wait -- )
    mov [dsp--],W2
    mov [dsp--],W1
    mov [dsp--],W0
_beep:
    push.D W4
    mov W0,W4
    mov W1,W5
    mov #(AUDIO_CLK>>1)&0xffff,W0
    mov #(AUDIO_CLK)>>17,W1
    repeat #17
    div.ud W0,W4
    dec W0,W0
    mov W0, AUDIO_R
    mov W0, AUDIO_RS
    mov #TOGGLE_MODE, W0
    ior AUDIO_OCCON1
    mov W5, _tone_len
    cp0 W2
    bra z, 1f
0:
    cp0 _tone_len
    bra nz, 0b
1:  
    pop.D W4
    return
