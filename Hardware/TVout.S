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

; vidéo NTSC B/W sur PIC24FJ64G002
; T2 période ligne horizontale
; OC1 sortie sync  sur RPB6
; OC2 contrôle début sortie vidéo
; sortie vidéo sur RPB5

.include "p24EP512MC202.inc"
#define __asm
#include "hardware.h"
#undef __asm

    
#include "ntsc_const.h"

.equ VOICE_SIZE, 8
.equ V_INCR, 0
.equ V_INDEX, 2
.equ V_ACCUM, 4 
.equ V_SAMPLE, 6    
.equ VARRAY, W2

.macro update_voice n
    mov [VARRAY+\n*VOICE_SIZE+V_INCR],W0
    mov [VARRAY+\n*VOICE_SIZE+V_ACCUM],W1
    add W0,W1,W0
    mov #0x7ff,W3
    and W3,W0,W3
    mov W3,[VARRAY+\n*VOICE_SIZE+V_ACCUM]
    lsr W0,#11,W0
    mov [VARRAY+\n*VOICE_SIZE+V_INDEX],W1
    add W0,W1,W1
    and #0xff,W1
    mov W1,[VARRAY+\n*VOICE_SIZE+V_INDEX]
    mov #samples,W3
    mov [VARRAY+\n*VOICE_SIZE+V_SAMPLE],W0
    sl W0,W0
    mov [W3+W0],W2
    mov.b [W2+W1],W0
    ze W0,W0
    add summer
.endm
    
.section .ndata data, near 
.global  _top_line, _hdelay, _dis_video 
;_T2counter: .space 2   ; compte les interruptions T2
line_count: .word 0xffff
 _top_line: .word TOPLINE
even: .byte 0xff
_dis_video: .byte 0  ; disable video
;; 4 canaux audio 
;.global _voices 
;_voices: 
;v0_incr: .word 0x0
;v0_index: .word 0
;v0_accum: .word 0
;v0_sample: .word 3
;
;v1_incr: .word 0x4000
;v1_index: .word 0
;v1_accum: .word 0
;v1_sample: .word 0
;
;v2_incr: .word 0
;v2_index: .word 0
;v2_accum: .word 0
;v2_sample: .word 0
;
;v3_incr: .word 0x
;v3_index: .word 0
;v3_accum: .word 0
;v3_sample: .word 0
;
; 
;;registre addition des échantillons
;summer:
;    .space 2
;    
;; table des échantillons sonore
;samples:
;    .word edsoffset(square_wave)
;    .word edsoffset(sawtooth_wave)
;    .word edsoffset(triangle_wave)
;    .word edsoffset(sine_wave)
    
    
 .section .video bss address(0x8000)
.global _video_buffer
_video_buffer: 
.space TV_BUFFER

;.section audio_samples ,psv, address(0x10000)
;
;square_wave:
;    .byte 127,64,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,64
;    .byte 127,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255     
;    .byte 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255     
;    .byte 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255     
;    .byte 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255     
;    .byte 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255     
;    .byte 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255     
;    .byte 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255     
;    .byte 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255     
;
;sine_wave:
;    .byte 127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173
;    .byte 176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215
;    .byte 217,219,221,223,225,227,229,231,233,234,236,238,239,240,242,243
;    .byte 244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254
;    .byte 254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245
;    .byte 244,243,242,240,239,238,236,234,233,231,229,227,225,223,221,219
;    .byte 217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178
;    .byte 176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130
;    .byte 127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81
;    .byte 78,76,73,70,67,64,62,59,56,54,51,49,46,44,42,39
;    .byte 37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11
;    .byte 10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0
;    .byte 0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9
;    .byte 10,11,12,14,15,16,18,20,21,23,25,27,29,31,33,35
;    .byte 37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76
;    .byte 78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124
;    
;    
;sawtooth_wave:
;    .byte 127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142
;    .byte 143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158
;    .byte 159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174
;    .byte 175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190
;    .byte 191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206
;    .byte 207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222
;    .byte 223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238
;    .byte 239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254
;    .byte 255,127,64,0,1,2,3,4,5,6,7,8,9,10,11,12
;    .byte 13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28
;    .byte 29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44
;    .byte 45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60
;    .byte 61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76
;    .byte 77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92
;    .byte 93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108
;    .byte 109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124
;    
;    
;triangle_wave:
;    .byte 127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142
;    .byte 143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158
;    .byte 159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174
;    .byte 175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190
;    .byte 191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206
;    .byte 207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222
;    .byte 223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238
;    .byte 239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254
;    .byte 255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240
;    .byte 239,238,237,236,235,234,233,232,231,230,229,228,227,226,225,224
;    .byte 223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208
;    .byte 207,206,205,204,203,202,201,200,199,198,197,196,195,194,193,192
;    .byte 191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176
;    .byte 175,174,173,172,171,170,169,168,167,166,165,164,163,162,161,160
;    .byte 159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144
;    .byte 143,142,141,140,139,138,137,136,135,134,133,132,131,130,129,128
    

.text

;********************
; interruption TIMER2
;********************
.global __T2Interrupt
__T2Interrupt:
    push W0
;    push.D W2
;    push DSRPAG
;;*** générateur audio **
;    mov line_count,W0
;    inc W0,W0
;    cp W0,#18
;    bra gt, 1f
;    btsc W0,#0
;    bra 2f
;1:
;    mov #_voices,VARRAY ; pointeur sur la table des canaux audio
;    mov #edspage(square_wave),W0
;    mov W0, DSRPAG
;    clr summer
;//    update_voice 0
;    update_voice 1
;//    update_voice 2
;//    update_voice 3
;    mov summer, W0
;    sl W0,W0
;    mov W0, AUDIO_R
;;********************    
;2:  pop DSRPAG
    inc line_count
    bra z, 1f
    mov line_count, W0
    cp W0, #6
    bra z, 2f
    cp W0,#12
    bra z, 3f
    cp W0, #18
    bra z, 4f
    mov _top_line, WREG
    cp line_count
    bra z, 5f
    add #VIDEO, W0
    cp line_count
    bra z, 6f
    mov #ELPFRM, W0
    cp line_count
    bra z, 7f
    mov #OLPFRM, W0
    cp line_count
    bra z, 8f
0:
    bclr IFS0, #T2IF
;    pop.D W2
    pop W0
    retfie
1: ; line_count==0 début VSYNC 6 demi-ligne, pulse étroit
    mov #SERATION, W0
    mov W0, OC1R
    mov #HALFLINE, W0
    mov W0, OC1RS
    mov W0, PR2
    bra 0b
2: ; line_count==6 , 6 demi-ligne, pulse large
    mov #(HALFLINE-SERATION), W0
    mov W0, OC1R
    bra 0b
3: ; line_count==12 , 6 demi-ligne , pulse étroit
    mov #SERATION, W0
    mov W0, OC1R
    bra 0b
4: ; line_count==18 , fin de VSYNC
    mov #HSYNC, W0
    mov W0, OC1R
    mov #HLINE, W0
    mov W0, OC1RS
    mov W0, PR2
    bra 0b
5: ; line_count==TOPLINE  activation interruption video
    cp0.b _dis_video
    bra nz, 0b
    bclr IFS0, #OC2IF
    bset IEC0, #OC2IE
    bra 0b
6: ; line_count==TOPLINE+VIDEDO désactivaion int. video
    bclr IEC0, #OC2IE
    bra 0b
7: ; line_count==nombre de ligne par frame pair
    clr W0
    cp.b even
    bra z, 0b
    setm line_count
    clr.b even
    bra 0b
8: ;line_count==nombre de ligne par frame impair
    setm line_count
    setm.b even
    bra 0b
    
;*********************
; interruption OC2
;*********************
.global __OC2Interrupt
__OC2Interrupt:
    push.D W0
    push.D W2
    push W4
    push RCOUNT
    push DSRPAG
    movpag #edspage(_video_buffer), DSRPAG
    ; calcul adresse de départ
    mov _top_line, WREG
    mov line_count, W2
    sub W2, W0,W2
    mov #BPL, W3
    mul.uu W2,W3, W0
    mov #edsoffset(_video_buffer), W1
    add W0, W1, W1
    mov LATB, W3
    mov #0xfff0, W2
    and W2,W3,W3
    mov #BPL, W4
    mov.b [W1++], W0
    ze W0,W0
1: ; boucle d'envoie des pixels.
    lsr W0, #4, W2
    ior W2,W3,W2
    mov W2,LATB
    repeat #4
    nop
    and #0xf,W0
    ior W0,W3,W2
    mov.b [W1++], W0
    ze W0,W0
    dec W4,W4
    mov W2,LATB
    repeat #4
    nop
    bra nz, 1b
    ; délais durée dernier pixel
    repeat #5
    nop
    mov W3, LATB
    bclr IFS0, #OC2IF
    pop DSRPAG
    pop RCOUNT
    pop W4
    pop.D W2
    pop.D W0
    retfie


.end

