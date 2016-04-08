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
 *  functions graphiques
 */

#define __asm    
#include "Hardware/ntsc_const.h"
#include "stackvm.h"
#include "font6x8.h"    
#undef __asm

.text 
    
.global _set_pixel, _draw_pixel
 _set_pixel: ; ( x,y,color -- )
    mov [dsp--],W2 ; couleur
    mov [dsp--],W1 ; y
    mov [dsp--],W0 ; x
_draw_pixel:
    push DSRPAG
    push DSWPAG
    movpag #1, DSRPAG
    movpag #1, DSWPAG
    cp W1, #YRES
    bra geu, 2f
    cp W0, #XRES
    bra geu, 2f
    and #0xf,W2
    mov W0,W3
    mov #BPL,W0
    mul.uu W1,W0,W0 ; y*BPL
    lsr W3,W1   ; x/2
    add W0,W1,W1  ; y*BPL+(x/2)
    mov #edsoffset(_video_buffer),W0
    add W1,W0,W1   ; video_buffer[y*BPL+(x/2)]
    mov.b [W1],W0  
    btsc W3,#0
    bra oddx
evenx:    
    and #0xf,W0
    swap.b W2
    ior.b W2,W0,W0
    bra 1f
oddx:
    and #0xf0,W0
    ior.b W2,W0,W0
1:
    mov.b W0,[W1]
2:    
    pop DSWPAG
    pop DSRPAG
    return

; retourne couleur du pixel
; à la position x,y
.global _vm_get_pixel
_vm_get_pixel: ; ( x, y -- c )
    mov [dsp--],W1 ; y
    mov [dsp], W0  ; x
    push DSRPAG
    movpag #1, DSRPAG
    mov #BPL,W2
    mul.uu W1,W2,W2 ; W2=y*BPL
    mov W0,w3
    lsr W0,W0
    add W0,W2,W0 ; W0=y*BPL+(x/2)
    mov #edsoffset(_video_buffer),W1
    add W0,W1,W0
    mov.b [W0],W0
    btss W3,#0
    swap.b W0
    and #0xf,W0
    mov W0,[dsp]
    pop DSRPAG
    return
    
;inverse la valeur d'un pixel    
.global _vm_xor_pixel, _xor_pixel
_vm_xor_pixel: ; ( x,y, xor -- )
    mov [dsp--],W2 ; xor
    mov [dsp--],W1 ; y
    mov [dsp--],W0 ; x
_xor_pixel:    
    push DSRPAG
    push DSWPAG
    movpag #1, DSRPAG
    movpag #1, DSWPAG
    mov W0,W3
    mov #BPL,W0
    mul.uu W1,W0,W0 ; position ligne
    lsr W3,W1  ; position octet sur la ligne
    add W1,W0,W0 ; position octet dans _video_buffer
    mov #edsoffset(_video_buffer),W1
    add W0,W1,W1
    mov.b [W1],W0
    and #15,W2
    btss W3,#0
    swap.b W2
    xor W2,W0,W0
    mov.b W0,[W1]
    pop DSWPAG
    pop DSRPAG
    return

    

; défilement de l'écran vers le haut
; de n lignes
.global _vm_scroll_up, _scroll_up
_vm_scroll_up: ; ( n -- )
    mov [dsp--],W0
_scroll_up:
    push DSRPAG
    push DSWPAG
    cp0 W0
    bra z, 2f
    cp W0, #YRES
    bra geu, 2f
    movpag #1, DSRPAG
    movpag #1, DSWPAG
    mov #BPL, W1
    mul.uu W1,W0,W0
    mov W0,W3   ;position source dans _video_buffer
    ; W2 source, W1 destination
    mov #edsoffset(_video_buffer),W1
    mov W1,W2
    add W0,W2,W2
    mov #TV_BUFFER,W0
    sub W0,W3,W0  ; nombre d'octets à déplacés
    dec W0,W0
    repeat W0 
    mov.b [W2++],[W1++]
    ; nettoyage fin _tv_buffer
    mov #_bg_color, W0
    mov.b [W0],W0
    and #0xf, W0
    sl W0,#4,W2
    ior W0,W2,W0
    dec W3,W3
    repeat W3
    mov.b W0,[W1++]
2:    
    pop DSWPAG
    pop DSRPAG
    return
    

; défilement de l'écran vers le bas
; de n lignes
.global _vm_scroll_down, _scroll_down
_vm_scroll_down: ; ( n -- )
    mov [dsp--],W0
_scroll_down:
    push DSRPAG
    push DSWPAG
    cp0 W0
    bra z, 2f
    cp W0, #YRES
    bra geu, 2f
    movpag #1,DSRPAG
    movpag #1,DSWPAG
    mov #BPL, W1
    ; la copie se fait de la fin vers le début
    mul.uu W1,W0,W0 ; distance destination à partir fin _video_buffer
    mov W0,W3
    ;W2 source, W1 destination
    mov #edsoffset(_video_buffer),W1 
    mov #TV_BUFFER,W2
    add W2,W1,W1 ; destination fin du _video_buffer
    sub W1,W3,W2 ; source
    mov #TV_BUFFER, W0
    sub W0,W3,W0 ; nombre d'octets à copier
    dec W0,W0
    repeat W0
    mov.b [--W2],[--W1]
    ; nettoyage début _tv_buffer
    mov #_bg_color, W0
    mov.b [W0],W0
    and #0xf, W0
    sl W0,#4,W2
    ior W0,W2,W0
    dec W3,W3
    repeat W3
    mov.b W0,[--W1]
2:    
    pop DSWPAG
    pop DSRPAG
    return

; défilement de l'écran vers la droite
; de n pixel, n doit-être pair.
; W2 source
; W1 dest
; W3 décalage octets/ligne
; W4 compteur de ligne
; W5 nombres d'octest à copier/lignes    
.global _vm_scroll_right
_vm_scroll_right: ; ( n -- )
    push DSRPAG
    push DSWPAG
    movpag #1, DSRPAG
    movpag #1, DSWPAG
    mov [dsp--],W0
    cp W0, #HRES
    bra ltu, 0f
erase_screen:    
    mov.b _bg_color,WREG
    and #0xf,W0
    sl W0,#4,W2
    ior W0,W2,W2
    mov #edsoffset(_video_buffer),W1
    repeat #TV_BUFFER-1
    mov.b W2,[W1++]
scroll_exit:    
    pop DSWPAG
    pop DSRPAG
    return
0:
    lsr W0,W3 ; octest décalage/ligne
    mov #BPL,W0
    sub W0,W3,W5 ; nombre d'octets à copier
    clr W4 ; compteur lignes
1:  ; boucle lignes
    mov #BPL, W0
    mul.uu W0,W4,W0
    add #BPL, W0
    mov #edsoffset(_video_buffer),W1
    add W0,W1,W1
    sub W1,W3,W2  ; décalage source
    mov W5, W0  ; nombre d'octets à transmettre
    dec W0,W0
    repeat W0
    mov.b [--W2],[--W1]
    mov.b _bg_color,WREG
    and #0xf,W0
    sl W0,#4,W2
    ior W0,W2,W2
    mov W3,W0
    dec W0,W0
    repeat W0
    mov.b W2,[--W1]
    inc W4,W4
    cp W4,#YRES
    bra ltu, 1b
    bra scroll_exit

; défilement de l'écran vers la gauche
; de n pixels, n doit-être pair
; W2 source
; W1 dest
; W3 décalage octets/ligne
; W4 compteur de ligne
; W5 nombres d'octest à copier/ lignes    
.global _vm_scroll_left
_vm_scroll_left: ; ( n -- )
    push DSRPAG
    push DSWPAG
    movpag #1, DSRPAG
    movpag #1, DSWPAG
    mov [dsp--],W0
    cp W0, #HRES
    bra geu, erase_screen
    lsr W0,W3 ; décalage en octets
    mov #BPL, W0
    sub W0,W3,W5 ; nombre d'octets à copier
    clr W4 ; compteur lignes
1:  ; boucle lignes
    mov #BPL, W0
    mul.uu W0,W4,W0
    mov #edsoffset(_video_buffer),W1 
    add W0,W1,W1  ; destination
    add W1,W3,W2 ; source
    mov W5,W0
    dec W0,W0
    repeat W0
    mov.b [W2++],[W1++]
    mov.b _bg_color,WREG
    and #0xf,W0
    sl W0,#4,W2
    ior W0,W2,W2
    mov W3,W0
    dec W0,W0
    repeat W0
    mov.b W2,[W1++]
    inc W4,W4
    cp W4, #YRES
    bra ltu, 1b
    bra scroll_exit


;ligne verticale
    ; W0 x 
    ; W1 y supérieur
    ; W2 longueur
    ; W3 couleur
;vline:
;    push.D W6
;    mov W0,W4
;    mov W1,W5
;    add W1,W2,W6
;    mov W3,W7
;0:
;    cp W5,W6
;    bra z, 1f
;    mov W5,W1
;    mov W4,W0
;    mov W7,W2
;    call _draw_pixel
;    inc W5,W5
;    bra 0b
;1:    
;    pop.D W6
;    return
    
;ligne horizontale
    ; W0 x gauche
    ; W1 y 
    ; W2 longueur
    ; W3 couleur
;hline:
;    push.D W6
;    mov W0,W4
;    mov W1,W5
;    add W0,W2,W6
;    mov W3,W7
;0:
;    cp W4,W6
;    bra z, 1f
;    mov W4,W0
;    mov W5,W1
;    mov W7,W2
;    call _draw_pixel
;    inc W4,W4
;    bra 0b
;1:    
;    pop.D W6
;    return
    
.global _vm_line
_vm_line: ; ( x0 y0 x1 y1 c -- )
    push W4
    mov [dsp--],W4
    mov [dsp--],W3
    mov [dsp--],W2
    mov [dsp--],W1
    mov [dsp--],W0
    callxfunc _draw_line
    pop W4
    return
    
.global _vm_sprite
_vm_sprite: ; ( x y w h sprite* -- )
    push W4
    mov [dsp--],W4
    mov [dsp--],W3
    mov [dsp--],W2
    mov [dsp--],W1
    mov [dsp--],W0
    callxfunc _draw_sprite
    mov W0,[++dsp]
    pop W4
    return
    
.global _box
; varibles locales    
.equ X0, 0*CELL_SIZE
.equ Y0, 1*CELL_SIZE
.equ XLIMIT, 2*CELL_SIZE
.equ YLIMIT, 3*CELL_SIZE
.equ BCOLOR, 4*CELL_SIZE
.equ CNTR, step
.equ LIMIT, limit
    
_box: ; ( x0,y0,w,h,c -- )  coin supérieur-gauche, largeur, hauteur, couleur
    ; ajuste frame pour les arguments
    mov fp, [++rsp]
    mov.D CNTR,[++rsp]
    mov dsp, fp
    mov #4*CELL_SIZE, W0
    sub fp,W0,fp
    mov [fp+X0],CNTR
    btsc CNTR,#15
    clr CNTR
    mov CNTR,[fp+X0]
    mov [fp+XLIMIT],LIMIT
    add CNTR,LIMIT,LIMIT
    mov #XRES,W0
    cp W0,LIMIT
    bra ge, 1f
    mov W0, LIMIT
1:  mov LIMIT,[fp+XLIMIT]
    mov [fp+Y0],CNTR
    btsc CNTR,#15
    clr CNTR
    mov CNTR,[fp+Y0]
    mov [fp+YLIMIT],LIMIT
    add CNTR,LIMIT,LIMIT
    mov #YRES,W0
    cp W0,LIMIT
    bra ge, 2f
    mov W0, LIMIT
2:  mov LIMIT,[fp+YLIMIT]
yloop:
    cp CNTR,LIMIT
    bra z, box_completed
    mov CNTR, [fp+Y0]
    mov [fp+X0],CNTR
    mov [fp+XLIMIT],LIMIT
xloop:
    cp CNTR,LIMIT
    bra z, row_completed
    mov CNTR, [++dsp] ; ( x -- )
    mov [fp+Y0],W0    
    mov W0,[++dsp]    ; ( x -- x y )
    mov [fp+BCOLOR],W0
    mov W0,[++dsp]    ; ( x y -- x y c )
    rcall _set_pixel   ; ( x y c -- )
    inc CNTR,CNTR
    bra xloop
row_completed:
    mov [fp+Y0],CNTR
    mov [fp+YLIMIT],LIMIT
    inc CNTR,CNTR
    bra yloop
box_completed:    
    ; jette arguments et variables locales
    mov.D [rsp--],CNTR
    mov [rsp--],fp
    mov #5*CELL_SIZE,W0
    sub dsp,W0,dsp
    return
    
.global _vm_rect
_vm_rect: ; ( x0,y0,width,height,color -- )
    push W4
    mov [dsp--],W4  ; color
    mov [dsp--],W3  ; height
    mov [dsp--],W2  ; width
    mov [dsp--],W1  ; top
    mov [dsp--],W0  ; left
    add W0,W2,W2    
    dec W2,W2       ;right
    add W1,W3,W3    
    dec W3,W3       ;bottom
    callxfunc _draw_rect
    pop W4
    return

.global _clreol
; efface la fin de la ligne
; à partir de la position du curseur    
    ; W2 pointeur vers _video_buffer
    ; W3 octet couleur de fond à copier dans _video_buffer
    ; W4 compteur ligne vidéo
    ; W5 limite ligne
    ; W6 offset sur la ligne
    ; W7 nombre d'octets à copier
_clreol: ; ( -- )
    push DSWPAG
    push.D W6
    movpag #edspage(_video_buffer), DSWPAG
    mov _xpos,W0
    lsr W0,W6	  ; offset sur la ligne
    mov #BPL,W7   
    sub W7,W6,W7  ; nombre d'octets à copier
    cp0 W7
    bra z, 1f
    dec W7,W7     ; décrémente pour le repeat
    mov _ypos, W4  ; compteur ligne
    mov #CHAR_HEIGHT,W5
    add W4,W5,W5  ; limite ligne
    mov.b _bg_color, WREG
    and #0xf,W0
    mov W0,W3
    swap.b W3
    ior W0,W3,W3 ; couleur fond
0:  ; boucle ligne
    mov #edsoffset(_video_buffer),W2
    mov #BPL, W1
    mul.uu W1,W4,W0
    add W0,W2,W2
    add W6,W2,W2
    repeat W7
    mov.b W3,[W2++]
    inc W4,W4
    cp W4,W5
    bra ltu, 0b
1:    
    pop.D W6
    pop DSWPAG
    return

    
.global _cls    
_cls:  ; ( -- )
    push DSWPAG
    movpag #1, DSWPAG
    mov.b _bg_color, WREG
    and #0xf, W0
    sl W0,#4,W1
    ior W0,W1,W0
    mov #edsoffset(_video_buffer),W1
    repeat #TV_BUFFER-1
    mov.b W0,[W1++]
    clr _xpos
    clr _ypos
2:
    pop DSWPAG
    return
 
.global _gray_scale
; échelle de gris
; W0 top
; W1 height    
_gray_scale:
    push W6
    mov W0, W4 ; y
    add W0,W1,W5 ; bottom
0:
    cp W4,w5
    bra z, 2f
    clr W6
1:
    mov W6,W0
    mov W4,W1
    lsr W0,#4,W2
    rcall _draw_pixel
    inc W6,W6
    cp W6,#HRES
    bra ltu, 1b
    inc W4,W4
    bra 0b
2:    
    pop W6
    return
 
; retourne la position ligne curseur texte    
.global _text_line
_text_line: ; ( -- )    
    mov _ypos,W0
    mov #CHAR_HEIGHT,W2
    repeat #17
    div.u W0,W2
    and #0xff,W0
    return
    
; retourne la position colonne curseur texte
.global _text_colon
_text_colon: ; ( -- )
    mov _xpos,W0
    mov #CHAR_WIDTH,W2
    repeat #17
    div.u W0,W2
    and #0xff,W0
    return

; retour à la ligne suivante
.global _new_line
_new_line: ; ( -- )
    mov #CHAR_HEIGHT,W0
    add _ypos,WREG
    cp W0, #VRES-CHAR_HEIGHT+1
    bra gtu, 1f
    rcall _clreol
    mov #CHAR_HEIGHT, W0
    add _ypos
    bra 2f
1:
    mov #CHAR_HEIGHT, W0
    rcall _scroll_up
2:    
    clr _xpos
    return
    
    