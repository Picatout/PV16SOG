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
    .word 69; DO2#    0
    .word 73; RE2     1
    .word 78; RE2#    2
    .word 82; MI2     3
    .word 87; FA2     4
    .word 92; FA2#    5
    .word 98; SOL2    6
    .word 104; SOL2#  7
    .word 110; LA2    8
    .word 117; LA2#   9
    .word 123; SI2    10
    .word 131; DO3    11
    .word 139; DO3#   12
    .word 147; RE3    13
    .word 156; RE3#   14
    .word 165; MI3    15
    .word 175; FA3    16
    .word 185; F3#    17
    .word 196; SOL3   18
    .word 208; SOL3#  19
    .word 220; LA3    20
    .word 233; LA3#   21
    .word 247; SI3    22
    .word 262; DO4    23
    .word 277; DO4#   24
    .word 294; RE4    25
    .word 311; RE4#   26
    .word 330; MI4    27
    .word 349; FA4    28
    .word 370; FA4#   29
    .word 392; SOL4   30
    .word 415; SOL4#  31
    .word 440; LA4    32
    .word 466; LA4#   33
    .word 494; SI4    34
    .word 523; DO5    35
    .word 554; DO5#   36
    .word 587; RE5    37
    .word 622; RE5#   38
    .word 659; MI5    39
    .word 698; FA5    40
    .word 740; FA5#   41
    .word 784; SOL5   42
    .word 831; SOL5#  43
    .word 880; LA5    44
    .word 932; LA5#   45
    .word 988; SI5    46
    .word 1046; DO6   47


_tone: // ( n1 n2 n3 --  ) n1->note, n2->durée, n3->wait
    mov [dsp-4],W0
    cp W0,#SCALE_SIZE
    bra ltu, 1f
    mov [dsp-2],W0
    sub dsp,#3*CELL_SIZE,dsp
    bra _pause
    
1:
    push DSRPAG
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
    mov [dsp--],W2 ; wait
    mov [dsp--],W1 ; len
    mov [dsp--],W0 ; freq
_beep:
    cp0 W1
    bra z, 1f
    mov W2,W3
    mov W0,W2
    mov W1,_tone_len
    mov #(AUDIO_CLK>>1)&0xffff,W0
    mov #(AUDIO_CLK)>>17,W1
    repeat #17
    div.ud W0,W2
    dec W0,W0
    mov W0, AUDIO_R
    mov W0, AUDIO_RS
    mov #TOGGLE_MODE, W0
    ior AUDIO_OCCON1
    cp0 W3
    bra z, 1f
0:
    cp0 _tone_len
    bra nz, 0b
1:  
    return
