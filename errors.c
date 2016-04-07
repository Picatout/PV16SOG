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

#include "text.h"
#include "errors.h"

__eds__ static const char*  __attribute__((space(prog))) ERR_MSG[]={
    "",// no error
    "Memory allocation.",
    "No SD card detected.",
    "Partition access.",
    "FAT access.",
    "Directory access.",
    "File not found.",
    "File access.",
    "File seek.",
    "File read.",
    "File write.",
    "File create.",
    "File too big.",
    "Address exception.",
    "0 division exception.",
    "DMA excpetion.",
    "Math exception.",
    "Stack exception.",
    "clavier PS/2 absent!",
};



void error(err_code_t error_code){
    char msg[40];
    
    if (error_code){
        new_line();
        print("ERROR: ");
        pstrcpy(msg,ERR_MSG[error_code]);
        prompt(msg,"*");
    }
}//f()

extern void fs_umount();
void fatal(err_code_t error_code){
    char msg[40];
    if (error_code){
        new_line();
        print("FATAL ERROR: ");
        pstrcpy(msg,ERR_MSG[error_code]);
        prompt(msg,"*");
        fs_umount();
        asm("reset");
    }
}

