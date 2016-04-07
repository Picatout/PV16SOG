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
 *  NAME: stackvm.h
 * DESCRIPTION: basic bytecode execution virtual machine.
 * DATE: 2016-02-14
 */


#ifndef STACKVM_H_
#define STACKVM_H_

#ifdef __cplusplus
extern "C"{
#endif

#define low(n) ((uint16_t)(n)&0xff)
#define high(n)(((uint16_t)(n)>>8)&0xff)
#define splitn(n)  low(n),high(n)
    
#define CELL_SIZE 2 // 2 octets.

#define DSTACK_SIZE (64)
#define RSTACK_SIZE (32)

    

#ifndef __asm

typedef void (*fnptr)();

extern uint8_t *progspace; // espace programme.
extern unsigned dp; //data pointer
extern void* endmark;
extern int vm_exit_code;
extern volatile uint16_t pause_timer;
extern int dstack[DSTACK_SIZE];
extern int rstack[RSTACK_SIZE];
extern int8_t dsp;
extern int8_t rsp;

extern void interpret(uint8_t *program);
extern void pause(uint16_t msec);

// fonctions dans vm_graphics.S
void draw_pixel(int x, int y, int color);
void scroll_up(unsigned int lines);
void scroll_down(unsigned int lines);
void cls();
void invert_pixel(int x, int y);
int get_pixel(int x, int y);
void gray_scale(int top, int height);


typedef enum {
    BYE,DROP,DROP2,DUP,SWAP,OVER,TUCK,PICK,TOR,RFROM,
    RFETCH,SAVELOOP,RESTLOOP,LOOPTEST,RND,ABS,BEEP,TONE,TICKS,SETPTMR,
    TSTPTMR,NOISE,CLS,LOCATE,BACK_COLOR,FONT_COLOR,EMIT,DOT,WKEY,TYPE,
    ACCEPT,SPACE,CRLF,PRTSTR,LIT,LITC,FETCH,FETCHC,STORE,STOREC,
    NEGATE,NOT,ADD,SUB,MUL,DIV,MOD,SHR,SHL,ONEPLUS,
    TWOPLUS,ONEMINUS,TWOMINUS,SQRT,EQUAL,GT,GE,LT,LE,BRANCH,
    ZBRANCH,NZBRANCH,CALL,LEAVE,DSPSTORE,DEPTH,STRCPY,DOTS,SAVESTEP,SAVELIMIT,
    NEXT,BTEST,ALLOC,LCSTORE,LCFETCH,LCADR,FRAME,LCVARSPACE,IDLE,JSTICK,
    SETPIXEL,GETPIXEL,INT,INVPIXEL,SCRLUP,SCRLDN,SCRLRT,SCRLLT,LINE,SPRITE,
    EXEC,EXIT,OR,XOR,AND,TRACE,ROT,BOX,KEY,RECT} bytecode_t;
    
#else
    
#define BYE (0)
#define DROP (BYE+1)
#define DROP2 (DROP+1)
#define DUP (DROP2+1)
#define SWAP (DUP+1)
#define OVER (SWAP+1)
#define TUCK (OVER+1)
#define PICK (TUCK+1)
#define TOR (PICK+1)
#define RFROM (TOR+1)
#define RFETCH (RFROM+1) 
#define SAVELOOP (RFETCH+1)
#define RESTLOOP (SAVELOOP+1)
#define LOOPTEST (RESTLOOP+1)
#define RND (LOOPTEST+1)
#define ABS (RND+1)
#define BEEP (ABS+1)
#define TONE (BEEP+1)
#define TICKS (TONE+1)
#define SETPTMR (TICKS+1)
#define GETPTMR (SETPTMR+1)
#define TSTPTMR (GETPTMR+1)
#define NOISE (TSTPTMR+1)
#define CLS (NOISE+1)
#define LOCATE (CLS+1)
#define BACK_COLOR (LOCATE+1)
#define FONT_COLOR (BACK_COLOR+1)
#define EMIT (FONT_COLOR+1)
#define DOT (EMIT+1)
#define WKEY (DOT+1)
#define TYPE (WKEY+1)
#define ACCEPT (TYPE+1)
#define SPACE (ACCEPT+1)
#define CRLF (SPACE+1)
#define PRTSTR (CRLF+1)
#define LIT (PRTSTR+1)
#define LITC (LIT+1)
#define FETCH (LITC+1)
#define FETCHC (FETCH+1)
#define STORE (FETCHC+1)
#define STOREC (STORE+1)
#define NEGATE (STOREC+1)
#define NOT (NEGATE+1)
#define ADD (NOT+1)
#define SUB (ADD+1)
#define MUL (SUB+1)
#define DIV (MUL+1)
#define MOD (DIV+1)
#define SHR (MOD+1)
#define SHL (SHR+1)
#define ONEPLUS (SHL+1)
#define TWOPLUS (ONEPLUS+1)
#define ONEMINUS (TWOPLUS+1)
#define TWOMINUS (ONEMINUS+1)
#define SQRT (TWOMINUS+1)
#define EQUAL (SQRT+1)
#define GE (EQUAL+1)
#define LT (GE+1)
#define LE (LT+1)
#define BRANCH (LE+1)
#define ZBRANCH (BRANCH+1)
#define NZBRANCH (ZBRANCH+1)
#define CALL (NZBRANCH+1)
#define LEAVE (CALL+1)
#define DSPSTORE (LEAVE+1)
#define DEPTH (DSPSTORE+1)
#define STRCPY (DEPTH+1)
#define DOTS (STRCPY+1)
#define SAVESTEP (DOTS+1)
#define SAVELIMIT (SAVESTEP+1)
#define NEXT (SAVELIMIT+1)
#define BTEST (NEXT+1)
#define ALLOC (BTEST+1)
#define LCSTORE (ALLOC+1)
#define LCFETCH (LCSTORE+1)
#define LCADR (LCFETCH+1)
#define FRAME (LCADR+1)
#define LCVARSPACE (FRAME+1)
#define IDLE (LCVARSPACE+1)
#define JSTICK (IDLE+1)
#define SETPIXEL (JSTICK+1)
#define GETPIXEL (SETPIXEL+1)
#define INT (GETPIXEL+1)
#define INVPIXEL (INT+1)
#define SCRLUP (INVPIXEL+1)
#define SCRLDN (SCRLUP+1)
#define SCRLRT (SCRLDN+1)
#define SCRLLT (SCRLRT+1)
#define LINE (SCRLLT+1)
#define SPRITE (LINE+1)
#define EXEC (SPRITE+1)
#define EXIT (EXEC+1)
#define OR (EXIT+1)
#define XOR (OR+1)
#define AND (XOR+1)
#define TRACE (AND+1)
#define ROT (TRACE+1)
#define BOX (ROT+1)
#define KEY (BOX+1)
#define RECT (KEY+1)
    
.equ step, W4   //FOR loop STEP
.equ limit, W5  // FOR loop limit
.equ ip, W6     // pointeur instructions bytecode
.equ wp, W7     // pointeur table des fonctions/bytecode
.equ dsp, W8    // pointeur pile arguments
.equ rsp, W9    // pointeur pile de contrôle
.equ fp, W10    // pointeur variables locales
.equ _page, W11 // sauvegarde DSRPAG 

.equ SRAM_SIZE, 65536

.equ CHAR_PER_LINE, 40
.equ DSTACK_ERROR, 1
.equ RSTACK_ERROR, 2
        
// ********* macros **************
    
.macro saveregs
    mov.D step, [W15++]
    mov.D ip, [W15++]
    push DSRPAG
    mov _page,DSRPAG
.endm

.macro restregs
    pop DSRPAG
    mov.D [--W15], ip
    mov.D [--W15], step
.endm
 
; appel fonction externe 'C'    
.macro callxfunc  f
    saveregs
    call \f
    restregs
.endm

;****************************************        
; macros pour accéder les champs de var_t
;****************************************        
; var.next dans W0
.macro var_next  var
    mov #\var,W0
.endm

; var.len dans W0
.macro var_len var
    mov #\var,W0
    inc2 W0,W0
    mov.b [W0],W0
.endm

; var.vtype dans W0
.macro var_vtype var
    mov #\var, W0
    add #3,W0
    mov.b [W0],W0
.endm

; var.name dans W0
.macro var_name var
    mov #\var,W0
    add #4, W0
    mov [W0],W0
.endm

; var.n|var.adr|var.str
; dans W0
.macro var_data var
    mov #\var,W0
    add #6, W0
    mov [W0],W0
.endm
    
#endif //__asm
    

#ifdef __cplusplus    
}
#endif
#endif // STACKVM_H_