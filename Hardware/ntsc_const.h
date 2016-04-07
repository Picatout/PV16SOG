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
 * File:   ntsc_const.h
 * Author: jacques
 *
 * Created on 19 août 2015 16:47
 * Description:  constantes utilisées par TVout.s et la fonction TVout_init()
 *               qui est dans hardware.c
 *
 * >>>>> les valeurs sont calculée pour un Fcy=16Mhz  <<<<<<<<
 *
 * référence standard NTSC: http://www.ntsc-tv.com/ntsc-index-02.htm
 */

#ifndef NTSC_CONST_H
#define	NTSC_CONST_H

#ifdef	__cplusplus
extern "C" {
#endif

//résolution d'image
#define XRES (240)
#define YRES (170)
#define BPL (XRES/2)
#define ROW YRES
#define TV_BUFFER (YRES*BPL)
#define VRES YRES
#define HRES XRES
#define ELPFRM  271  // nombre de lignes par frame pair
#define OLPFRM  272  // nombre de lignes par frame impair
#define VIDEO   ROW  // nombre de lignes vidéo

    

// constantes génération signal NTSC pour Fcy=35Mhz
#define HFREQ 15748 // fréquence horizontale pour information seulement
#define HLINE 4444//(2221) // PR2= période ligne horizontale
#define HSYNC 326//(163)  // OC1R=47e-6*FCY-1 sync pulse horizontal
#define HALFLINE 2222 //(1110) // période demi-ligne horizontale (VSYNC)
#define SERATION (HSYNC/2) //durée pulse dans VSYNC
#define TOPLINE 60  // première ligne visible valeur par défaut
#define MINTOP  33  // valeur minimale pour première ligne visible
#define MAXTOP  75  // valeur maximale pour première ligne visible    
#define VIDEO_DELAY (2*HSYNC+250)
#define ADJMIN (2*HSYNC+100) // valeur d'ajustement minimale délais vidéo
#define ADJMAX (ADJMIN+400) // valeur maximale ajustement video

#ifdef	__cplusplus
}
#endif

#endif	/* NTSC_CONST_H */

