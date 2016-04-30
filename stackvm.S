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
 *  NAME: stackvm.S
 * DESCRIPTION: machine virtuelle à pile exécutant le bytecode BASIC
 * DATE: 2016-02-14
 */

/*  NOTES:
 *    T -> valeur au sommet de dstack
 *    R ->  valeur au sommet de rstack
 */
/*
 *   utilisation des registres
 *   W6  ip
 *   W7  pointeur words
 *   W8  pointeur dstack
 *   W9  pointeur rstack
 *   W10 pointeur frame
 *   W11 valeur DSRPAG avant appel interpréteur
 *   W12  step
 *   W13  limit
  */

.include "p24EP512MC202.inc"  
#define __asm
#include "Hardware/hardware.h"
#include "Hardware/tones.h"
#include "stackvm.h"
#include "Hardware/ntsc_const.h" 
#include "Hardware/TVout.h"
#include "font6x8.h"  
#include "text.h"    
#undef __asm    
    

    
.global  _rstack,_rsp, _endmark, _progspace,_dp, _ftrace, _vm_exit_code

.section ndata bss near  
_dstack_empty:
.space 2    
_dstack:
.space DSTACK_SIZE
_rstack_empty:
.space 2    
_rstack:
.space RSTACK_SIZE    
_dsp: // pointejur pile données
.space 1
_rsp: // pointeur pile retours
.space 1
_endmark: ;adresse fin des variables
.space 2
_progspace: ; adresse espace programme
.space 2 
_dp:        ; pointer dans _progspace
.space 2
_pad:       ; tampon chaine lecture clavier
.space HRES/CHAR_WIDTH    
_ftrace:    ; indicateur mode traçage
.space 2
_vm_exit_code:  ; code de sortie de la machine virtuelle
.space 2

.global _pause_timer
_pause_timer: .space 2
 
last_bytecode: .space 2
 
.text
    
//*********************
// opération sur piles
//*********************

drop: //( n --)
    dec2 dsp,dsp
    return

dup: // ( n -- n n )
    mov [dsp],[++dsp]
    return


vm_swap: // ( n1 n2 -- n2 n1 )
    mov [--dsp],W0
    mov [++dsp],[--dsp]
    mov W0,[++dsp]
    return

rot: // ( n1 n2 n3 -- n3 n1 n2 )
    mov [dsp-4],W0
    mov [dsp-2],W1
    mov [dsp],W2
    mov W2, [dsp-4]
    mov W1,[dsp]
    mov W0,[dsp-2]
    return

.global over
over: // (n1 n2 -- n1 n2 n1 )
    mov [dsp-2],W0
    mov W0,[++dsp]
    return

// transfert l'élément au sommet de dstack
// dans limit
savelimit: //( limit -- )
    mov [dsp--],limit
    return

;sauvegarde les variables de boucle sur rstack
; sur _rstack
saveloop: // (R: -- step limit)
    mov step,[++rsp] //step
    mov limit,[++rsp] //limit
    return

// sauvegarde STEP
savestep: // ( step -- )
    mov [dsp--],step
    return

; restore les variables de boucle
; à partir de _rstack
restloop: // ( R:step R:limit -- )
    mov [rsp--],limit
    mov [rsp--],step
    return

//addresse de la variable de contrôle 
// au sommet de la pile
// n est la valeur de la variable de contrôle
// flag=0 si limite dépassée.
looptest: // (  n -- flag )
    clr W0
    sub limit,[dsp],W1  //test limit
    btss step,#15
    bra lt, 1f
    btsc step,#15
    bra gt, 1f
    setm W0
    1:
    mov W0,[dsp]
    return

// incrémente variable de la
// boucle FOR
// adresse de la variable au sommet de pile
// step contient la valeur de l'incrément
// adr -> adresse de la variable
// n -> valeur de la variable incrémentée
for_next: // ( adr -- n )
    mov [dsp],W0
    mov [W0],W1
    add step,W1,W1
    mov W1,[W0]
    mov W1,[dsp]
    return

;**************************
; gestion mémoire programme
;**************************

;allocation d'espace à la fin de _progspace
.global alloc
alloc:; ( size -- adr|NULL )  size -> nombre d'octets (arrondie à un nombre pair.
    mov [dsp],W0
    cp0 W0
    bra z, 0f
    sub _endmark,WREG
    btsc W0,#0
    dec W0,W0
    mov _progspace, W1
    mov _dp,W2
    add W1,W2,W1
    cp W0, W1
    bra gt, 1f
    clr [dsp]
0:  return
1:  mov W0, _endmark    
    mov W0, [dsp]
    return
    
    
    
//************************
// entrées/sorties
//************************
// imprime 1 caractère a l'écran
emit: // ( c -- )
    mov [dsp--],W0
    and #0x7f,W0
    callxfunc(_put_char)
    return

; chaîne litérale
lits: // ( -- char* )
    clr W0
    mov.b, [ip++],W0 ; longueur chaîne incluant zéro terminal
    mov ip, [++dsp]
    add W0,ip,ip
1:  
    return

//imprime la valeur dans W0
dotW0: // ( -- )
    push W0
    clr W1
    btsc W0,#15
    com W1,W1
    clr W2
    callxfunc(_print_int)
    mov #32,W0
    callxfunc(_put_char)
    pop W0
    return

// imprime T à l'écran
dot: // (n --)
    mov [dsp--],W0
    rcall dotW0
    return

// affiche le contenu de la pile
dots:
    rcall _new_line
    mov #'R',W0
    callxfunc _put_char
    mov #':',W0
    callxfunc _put_char
    mov #_rstack_empty,W0
    sub rsp,W0,W0
    asr W0,W0
    rcall dotW0
    rcall _new_line
    mov #_dstack_empty,W3
    sub dsp,W3,W0
    asr W0,W0
    rcall dotW0
    mov #':',W0
    callxfunc(_put_char)
    mov #32, W0
    callxfunc(_put_char)
    mov #_dstack,W3
0:  cp dsp,W3
    bra n, 1f
    mov [W3++],W0
    push W3
    rcall dotW0
    pop W3
    bra 0b
1:  return 

// lecture 1 caractère du clavier
wait_key: // ( -- c )
    callxfunc(_kbd_wait_key)
    and #255,W0
    mov W0,[++dsp]
    return

; vérifie s'il y a un caractère dans 
; retourne 0 si pas de caractère
key: ; ( -- key )
    callxfunc _kbd_get_key
    and #255,W0
    mov W0, [++dsp]
    return

    
// imprime une chaïne ZASCII à l'écran
//adresse sur la pile.
type: // ( adr -- )
    mov [dsp--],W0
    callxfunc(_print)
    return

// lecture d'une chaine au clavier
// n buffer size
// addresse buffer = _pad
// longueur chaine lue.    
accept:  // ( n -- adr len )
    mov #'?', W0
    callxfunc _put_char
    mov [dsp--],W0
    mov #HRES/CHAR_WIDTH, W1
    cp W0,W1
    bra gt, 1f
    mov W0,W1
1:  mov #_pad,W0
    mov W0,[++dsp]
    callxfunc _readln
    ze W0,W0
    mov W0,[++dsp]
    return

; convertie la chaine
; en entier
; adr -> adresse chaîne    
to_int: ; (adr -- n )
    push W6
    clr W6  ; signe
    clr W4  ; n
    mov #10, W2  ; base décimale
    mov [dsp--], W3
    mov.b [W3++],W0
    cp.b W0,#'$'
    bra neq, 1f
    mov #16,W2 ; base hexadécimale
    bra 3f
1:
    cp.b W0,#'#'
    bra neq, 2f
    mov #2,W2  ; base 2
    bra 3f
2:    
    cp.b W0,#'-'
    bra neq, 4f
    com W6,W6
3:
    mov.b [W3++],W0
4:
    ze W0,W0
    cp W0,#'a'
    bra ltu, 5f
    sub #32,W0
5:    
    cp W0,#'0'
    bra ltu, 8f
    sub #'0',W0
    cp W0,#9
    bra leu, 6f
    sub #7,W0
6:    
    cp W0,W2
    bra geu, 8f
    mul.uu W4,W2,W4
    add W0,W4,W4
    bra 3b 
8:
    cp0 W6
    bra eq, 9f
    neg W4,W4
9:    
    mov W4,[++dsp]
    pop W6
    return
    
//imprime un espace
space: // ( -- )
    mov #32,W0
    callxfunc(_put_char)
    return

//positionne le curseur texte
vm_locate: // ( line col -- )
    mov [dsp--],W2 ; col
    mov [dsp--],W0 ; line
    cp W0,#VRES/CHAR_HEIGHT
    bra geu, 1f    
    cp W2, #HRES/CHAR_WIDTH
    bra geu, 1f
    mov #CHAR_HEIGHT,W1
    mul.uu W0,W1,W0
    mov W0, _ypos
    mov #CHAR_WIDTH,W0
    mul.uu W2,W0,W0
    mov W0, _xpos
1:   
    return

//retourne la ligne du curseur texte
curline: ; ( -- n )
    mov _ypos,W0
    mov #CHAR_HEIGHT,W2
    repeat #17
    div.u W0,W2
    mov W0,[++dsp]
    return

; retourne la colonne du curseur texte    
curcol: ; ( -- n )
    mov _xpos,W0
    mov #CHAR_WIDTH,W2
    repeat #17
    div.u W0,W2
    mov W0,[++dsp]
    return
    
    
    
// change la couleur de fond d'écran
vm_back_color: // ( n -- )
    mov [dsp--],W0
    and #15,W0
    mov.b WREG,_bg_color
    return

//change la couleur de la police
vm_font_color: // ( n -- )
    mov [dsp--],W0
    and #15,W0
    mov.b WREG,_fg_color
    return


//************************
// constantes
//************************

// literal entier 16 bits
lit:  // ( -- n )
    mov.b [ip+1],W0
    swap W0
    mov.b [ip],W0
    mov W0,[++dsp]
    inc2 ip,ip
    return

// literal entier 8 bits
litc: // ( -- c )
    mov.b [ip++],W0
    ze W0,W0
    mov W0,[++dsp]
    return

//**************************
// variables
//***************************

//empile valeur variable entier 16 bits
fetch: // ( adr -- n )
    mov [dsp],W2
    mov [W2],[dsp]
    return

//empile variable entier 8 bits
fetchc: // ( adr -- c )
    mov [dsp],W0
    mov.b [W0],W0
    ze W0,W0
    mov W0,[dsp]
    return

//enregistre valeur variable 16 bits
store:  // ( n adr -- )
    mov [dsp--],W2
    mov [dsp--],[W2]
    return

//enregistre valeur variable 8 bits
storec: // ( c adr -- )
    mov [dsp--],W0
    mov.b [dsp--],[W0]
    dec dsp,dsp
    return

//sauvegarde variable local
//[IP] indique la position de la variable
local_store: // ( n -- )
    mov.b [ip++],W0
    ze W0,W0
    sl W0,W0
    mov [dsp--],[fp+W0]
    return

//empile variable locale
//[IP] indique la position  de la variable
local_fetch:
    mov.b [ip++],W0
    ze W0,W0
    sl W0,W0
    mov [fp+W0],[++dsp]
    return

//empile l'adresse de la variable locale
// [IP] indique la position de la variable
//sur la pile
local_address: //( -- adr )
    mov.b [ip++],W0
    ze W0,W0
    sl W0,W0
    add W0,fp,W0
    mov W0,[++dsp]
    return

//retourne la grandeur d'une table
// cette valeur précède le premier
// élément de la table    
ubound: ; ( adr -- size )
    mov [dsp],W0
    mov [W0],[dsp]
    return
    
    
//copie d'une chaîne zéro terminal
str_copy: // ( src dest -- )
    mov [dsp--],W0 ;dest
    mov [dsp--],W1 ; src
0:  cp0.b [W1]
    bra z, 1f
    mov.b [W1++],[W0++]
    bra  0b
1:  clr.b [W0]
    return

//**************************
// opérations logique et arithmétiques
//**************************

negate: //( n -- -n )
    neg [dsp],[dsp]
    return

not: // ( n -- !n )
    com [dsp],[dsp]
    return

vm_ior: ; ( n1 n2 -- n1|n2 )
    mov [dsp--],W0
    ior W0,[dsp],[dsp]
    return
    
vm_xor: ; ( n1 n2 -- n1^n2 )
    mov [dsp--],W0
    xor W0,[dsp],[dsp]
    return


vm_and: ; ( n1 n2 -- n1&n2 )
    mov [dsp--],W0
    and W0,[dsp],[dsp]
    return
    
vm_add: // ( n1 n2 -- n1+n2 )
    mov [dsp--],W0
    add W0,[dsp],[dsp]
    return

vm_sub: // ( n1 n2 -- n1-n2 )
    mov [dsp--],W0
    mov [dsp],W1
    sub W1,W0,[dsp]
    return

vm_mul: // ( n1 n2 -- n1*n2 )
    mov [dsp--],W0
    mov [dsp],W1
    mul.ss W1,W0,W0
    mov W0,[dsp]
    return

// division entière
divide: // ( n1 n2 -- n1/n2 )
    mov [dsp--],W2
    mov [dsp],W0
    repeat #17
    div.s W0,W2
    mov W0, [dsp]
    return

// division modulo
mod: // ( n1 n2 -- n1%n2 )
    mov [dsp--],W2
    mov [dsp],W0
    repeat #17
    div.s W0,W2
    mov W1, [dsp]
    return

// T>>1
shr: //( u -- u>>1 )
    asr [dsp],[dsp]
    return

// T<<1
shl: // ( u -- u<<1)
    sl [dsp],[dsp]
    return

 min: ; ( n1 n2 -- min(n1,n2) )
    mov [dsp--],W0
    cp W0,[dsp]
    bra ge, 1f
    mov W0,[dsp]
 1:   
    return

max: ; ( n1 n2 -- max(n1,n2) )
    mov [dsp--],W0
    cp W0, [dsp]
    bra le, 1f
    mov W0,[dsp]
1:    
    return
    
muldiv: ; ( n1 n2 n3 -- n ) n1*n2/n3  résultat intermédiaire sur 32 bits
    mov [dsp--],W4
    mov [dsp--],W0
    mov [dsp],W2
    mul.ss w0,w2,w0
    repeat #17
    div.sd W0,W4 
    mov W0,[dsp]
    return
    
//racine carree
//REF: https://fr.wikipedia.org/wiki/M%C3%A9thode_de_H%C3%A9ron
sqroot: //( u -- sqrt(u) )
    lsr [dsp],W2
    clr W3
    0: sub W2,W3,W1
    bra z, 1f
    dec W1,W1
    bra z, 1f
    mov [dsp],W0
    repeat #17
    div.u W0,W2
    mov W0,W3
    add w2,w3,w2
    lsr W2,W2
    bra 0b
    1: mov W2,[dsp]
    return

// retourne un nomber aléatoire
rnd: //( -- n)
    saveregs
    rcall _random
    restregs
    mov W0, [++dsp]
    return

vm_abs: //(n -- u )
    btst.c [dsp],#15
    bra nc, 1f
    neg [dsp],[dsp]
    1:
    return

// test bit #b de n    
btest: ; ( n b -- f )
    clr W0
    mov [dsp--],W1
    btst.z [dsp],W1
    bra z, 1f
    com W0,W0
1:  mov W0,[dsp]
    return
    
/*******************
 * fonctions variées
******************/

// empile _msec_ticks
vm_ticks:// ( -- u )
    mov _msec_ticks, W0
    mov W0, [++dsp]
    return

;vm_tone: // ( n1 n2 n3 --  ) n1->note, n2->durée, n3->wait
;    mov [dsp-4],W0
;    and #0x3f,W0
;    cp W0,#SCALE_SIZE
;    bra lt, 1f
;    return
;1:
;    sl W0,W0
;    mov #_scale,W1
;    mov [W1+W0],W0
;    mov W0,[dsp-4]
;    
;
;.extern _tone_len
;// génère un beep sonore
;vm_beep: // ( n1 n2 n3 -- ) n1->freq, n2-> durée, n3->wait
;    mov #(AUDIO_CLK>>1)&0xffff,W0
;    mov #(AUDIO_CLK)>>17,W1
;    mov [dsp-4],W2  // fréquence
;    repeat #17
;    div.ud W0,W2
;    dec W0,W0
;    mov W0, AUDIO_R
;    mov W0, AUDIO_RS
;    mov #TOGGLE_MODE, W0
;    ior AUDIO_OCCON1
;    mov [dsp-2],W1  // durée
;    mov W1, _tone_len
;    mov [dsp--],W2  //flag wait
;    cp0 W2
;    bra z, 1f
;0:
;    cp0 _tone_len
;    bra nz, 0b
;1:  
;    sub #4,dsp
;    return

    
.global _pause    
//suspend exécution pour n millisecondes 
vm_pause: // ( n -- )
    mov [dsp--],W0
_pause:
    mov W0, _pause_timer
1:
    cp0 _pause_timer
    bra nz, 1b
    return
    
// initialise pause_timer
set_tmr: // ( n1 -- ) n1-> durée en msec
    mov [dsp--],W0
    mov W0, _pause_timer
    return

// vérifie si pause_timer==0
timeout: // ( -- flag )
;    clr W0
;    mov W0, [++dsp]
    clr [++dsp]
    cp0 _pause_timer
    bra nz, 1f
    com [dsp],[dsp]
1:
    return

vm_noise:
    mov [dsp--],W0
    mov W0, _tone_len
0:  callxfunc _random
    mov #16,W1
1:  btsc W0,#0
    btg AUDIO_PORT,#AUDIO_OUT
    lsr W0,W0
    dec W1,W1
    bra nz, 1b
    cp0 _tone_len     
    bra nz, 0b
    return

//*************************
// joystick
//*************************
    
//lecture du joystick    
read_stick: //( -- n )
    mov JOYPORT, W0
    lsr W0,#11,W0
    xor #0x1f,W0
    mov W0, [++dsp]
    return

    
//*************************
// comparaison
//*************************

// n1==n2
isequal:// ( n1 n2 -- flag )
    clr W1
    mov [dsp--],W0
    cp W0,[dsp]
    bra nz, 1f
    setm W1
1:  
    mov W1,[dsp]
    return

// n1!=n2
isnotequal:
    clr W1
    mov [dsp--],W0
    cp W0,[dsp]
    bra z, 1f
    setm W1
1:
    mov W1,[dsp]
    return
    
// n1>n2
isgreater: // ( n1 n2 -- flag )
    setm W1
    mov [dsp--],W0
    cp W0,[dsp]
    bra n, 1f
    clr W1
1: 
    mov W1,[dsp]
    return

// n1>=n2
isgrequal: // ( n1 n2 -- flag )
    setm W1
    mov [dsp--],W0
    cp W0,[dsp]
    bra le, 1f
    clr W1
1: 
    mov W1,[dsp]
    return

// n1<n2
islittlethan: // ( n1 n2 -- flag)    
    setm W1
    mov [dsp--],W0
    cp W0,[dsp]
    bra gt, 1f
    clr W1
1:
    mov W1,[dsp]
    return

// n1<=n2
isltequal: // ( n1 n2 -- flag )
    setm W1
    mov [dsp--],W0
    cp W0,[dsp]
    bra ge, 1f
    clr W1
1: 
    mov W1,[dsp]
    return

//**************************
// branchements
//**************************

// branchement relatif inconditionnel
branch: // ( -- )
    mov.b [ip++],W0
    ze W0,W1
    mov.b [ip++],W0
    ze W0,W0
    swap W0
    add W0,W1,W0
    add W0,ip,ip
    return

// branchement relatif si T==0
zerobranch: // ( flag -- )
    mov.b [ip++],W0
    ze W0,W1
    mov.b [ip++],W0
    ze W0,W0
    swap W0
    add W0,W1,W0
    mov [dsp--],W1
    cp0 W1
    bra nz, 1f
    add W0,ip,ip
    1:
    return

// branchement relatif si T!=0
nzbranch: // ( n -- )
    mov.b [ip++],W0
    ze W0,W1
    mov.b [ip++],W0
    ze W0,W0
    swap W0
    add W0,W1,W0
    mov [dsp--],W1
    cp0 W1
    bra z, 1f
    add W0,ip,ip
    1:
    return

//crée l'espace sur dstack pour
// les variables locales
//[IP] nombre de variables locales
local_var: ; 
    clr W0
    mov.b [ip++],W0
    sl W0,W0
    add dsp,W0,dsp
    return

//ajuste le registe fp
//pour les arguments
// [IP] nombres d'arguments
frame:// (  --  )
    clr W0
    mov.b [ip++],W0
    sl W0,W0
1:  mov fp,[++rsp]
    sub dsp,W0,fp
    return

// appel sous-routine
enter: // ( --  )
    clr W0
    mov.b [ip++],W0
    mov W0,W1
    mov.b [ip++],W0
    swap W0
    mov ip, [++rsp]
    add W0,W1,W0
    mov [W0],ip
    return

// sortie sous-routine
leave: // (R: link -- )
    mov.b [ip++],W0
    ze W0,W0
    sl W0,W0
    sub dsp,W0,dsp
    mov [rsp--],fp
    mov [rsp--],ip
    return

; retourne la longueur de la chaine    
strlen: ; ( char* -- len )
    clr W0
    mov [dsp],W1
0:    
    cp0.b [W1++]
    bra z, 1f
    inc W0,W0
    bra 0b
 1:
    mov W0, [dsp]
    return
 
    
;***********************
;  accès SPIRAM
;***********************

; charge un fichier dans SPIRAM
; char* -> nom du fichier
; retourne: size -> nombre d'octets    
sr_load: ; ( char* -- size )
    mov [dsp],W0
    clr W1 ; sram_address=0
    mov #0xffff,W2 ; maxsize 65535
    callxfunc _fs_load_spiram ; (char*, sram_address, size)
    mov W0, [dsp]
    return
    
; sauvegarde SPIRAM dans fichier
; char* -> nom du fichier
; size -> nombre d'octets    
sr_save:; ( char* size  -- )
    mov [dsp--],W2 ; size
    clr W1         ; spi_ram address
    mov [dsp--],W0 ; file_name*
    callxfunc _fs_save_spiram
    return
    
; met à zéro un bloc de SPIRAM
sr_clear: ; ( adr size -- )
    mov [dsp--],W1
    mov [dsp--],W0
    callxfunc _sram_clear_block
    return

; lecture d'un bloc de mémoire SPIRAM
sr_read: ; ( adr buffer size -- )
    mov [dsp--],W2
    mov [dsp--],W1
    mov [dsp--],W0
    callxfunc _sram_read_block
    return

; copie d'une variable dans le mémoire SPIRAM    
sr_write: ; ( adr buffer size -- )
    mov [dsp--],W2
    mov [dsp--],W1
    mov [dsp--],W0
    callxfunc _sram_write_block
    return
    
    
bye: // ( -- )
    sub #4,W15
    bra vm_quit

;active/désactive mode TRACE
trace: // ( f -- )
    mov [dsp--],W0
    mov W0,_ftrace
    sub #4,W15
    cp0 W0
    bra z, trace_off
    bra trace_on
    
; vérification des limites de _rstack
rstack_bound_check:
    mov #_rstack_empty, W0
    cp rsp,W0
    bra ltu, 1f
    add #RSTACK_SIZE, W0
    cp rsp,W0
    bra geu, 1f
    return
1:  mov #RSTACK_ERROR, W0
    mov W0, _vm_exit_code
    sub #4,W15
    bra vm_quit
    
    
; vérification des limites de _dstack    
dstack_bound_check:
    mov #_dstack_empty, W0
    cp dsp,W0
    bra ltu, 1f
    add #DSTACK_SIZE, W0
    cp dsp,W0
    bra geu, 1f
    return
1:  mov #DSTACK_ERROR, W0
    mov W0, _vm_exit_code
    sub #4,W15
    bra vm_quit
    
    
//table des fonctions correspondant à bytecode_t
words:    
.word bye,drop,dup,vm_swap,over,saveloop,restloop,looptest,rnd,vm_abs
.word _vm_beep,_tone,vm_ticks,set_tmr,timeout,vm_noise,_cls,vm_locate,vm_back_color,vm_font_color
.word emit,dot,wait_key,type,accept,space,_new_line,lits,lit,litc
.word fetch,fetchc,store,storec,negate,not,vm_add,vm_sub,vm_mul,divide
.word mod,shr,shl,sqroot,isequal,isnotequal,isgreater,isgrequal,islittlethan,isltequal
.word branch,zerobranch,nzbranch,enter,leave,str_copy,dots,savestep,savelimit,for_next
.word btest,alloc,local_store,local_fetch,local_address,frame,local_var,vm_pause,read_stick,_set_pixel
.word _vm_get_pixel,to_int,_vm_xor_pixel,_vm_scroll_up,_vm_scroll_down,_vm_scroll_right,_vm_scroll_left,_vm_line,_vm_sprite,vm_ior
.word vm_xor,vm_and,trace,rot,_box,key,_vm_rect,strlen,sr_load,sr_save
.word sr_clear,sr_read,sr_write, min, max, muldiv, ubound,_video_off,curline,curcol
.word _save_scr,_rest_scr,_rem_sprite    
    
.global _interpret
_interpret:
    mov.D step, [W15++]
    mov.D ip, [W15++]
    mov.D dsp, [W15++]
    mov.D fp, [W15++]
    mov DSRPAG, _page
    mov W0, ip // ip
    movpag #edspage(words),DSRPAG
    mov #edsoffset(words),wp
    mov #_dstack_empty,dsp   // pointeur pile arguments
    mov #_rstack_empty,rsp   // pointeur pile de contrôle
    clr step // boucle valeur début
    clr limit // boucle valeur fin
    clr _vm_exit_code
    //vérification TRACE flags
    cp0 _ftrace
    bra z, trace_off
    // interpréteur mode trace
trace_on:
    ze [ip++],W0
    mov W0, last_bytecode
    sl W0,W0
    mov [wp+W0],W0
    call W0
    rcall dstack_bound_check
    rcall rstack_bound_check
    mov #SRAM_SIZE-(XRES/2)*YRES,W0
    callxfunc _save_screen
    rcall _cls
    mov last_bytecode,W0
    rcall dotW0
    rcall dots
    callxfunc _prompt_continue
    push W0
    mov #SRAM_SIZE-(XRES/2)*YRES,W0
    callxfunc _restore_screen
    pop W0
    cp.b W0,#27
    bra nz, trace_on
    clr _ftrace
    //interpréteur
trace_off:
    ze [ip++],W0
    sl W0,W0
    mov [wp+W0],W0
    call W0
    bra trace_off
    //sortie de l'interpréteur
    //sauvegarde la valeur de dsp
vm_quit:
    mov _page, DSRPAG
    mov.D [--W15],fp
    mov.D [--W15],dsp
    mov.D [--W15],ip
    mov.D [--W15],step
    return
	
.end
	

