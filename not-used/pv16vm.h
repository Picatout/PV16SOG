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
 * pv16vm.h
 *
 * Created: 2014-09-26 09:38:22
 *  Author: Jacques Deschênes
 */ 



#ifndef PV16VM_H_
#define PV16VM_H_

#define VM_CONTINUE (0)
#define VM_EXIT_OK  (1)
#define VM_BAD_OPCODE (2)
#define VM_BREAK (3)

#define F_RESET (2)
#define F_DEBUG (4)
#define F_TRACE (8)

typedef struct vm_state{
	uint16_t pc;
	uint16_t ix;
	uint8_t  sp;
	uint16_t  var[16];
	union {
		uint16_t opcode;	
	struct{	
		uint8_t b1;
		uint8_t b2;
		};
	};
	uint16_t stack[32];
	uint8_t  debug:1;
	uint8_t  trace:1;
	}vm_state_t;

#define PROG_SPACE (8192)

bool print_vms(const char *msg);
	
uint8_t pv16vm(uint8_t* program, uint8_t flags);
void set_break_point(uint16_t addr);
void clr_break_point(uint16_t addr);
#endif /* PV16VM_H_ */