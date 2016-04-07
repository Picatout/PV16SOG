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
 * filesys.h
 *
 * Created: 2014-10-11 21:09:04
 *  Author: Jacques
 */ 


#ifndef FILESYS_H_
#define FILESYS_H_

// sdcard and FAT library
#include "FAT/fat.h"
#include "FAT/partition.h"
#include "FAT/sd_raw.h"

#include "errors.h"


err_code_t fs_last_error();

err_code_t fs_mount();

void fs_umount();

err_code_t  fs_open_dir(const char *dir_name);

void fs_close_dir();

struct fat_file_struct *fs_open_file(const char *file_name);
uint16_t fs_read_file(struct fat_file_struct *fh,uint8_t* buffer, uintptr_t buffer_len);
uint16_t fs_write_file(struct fat_file_struct *fh,const uint8_t* buffer, uintptr_t buffer_len);
err_code_t fs_file_reset(struct fat_file_struct *fh);
#define fs_close_file(fh) fat_close_file(fh) //void fs_close_file(struct fat_file_struct *fh);
uint16_t fs_load_file(const char *file,uint16_t sram_address, uint16_t maxsize);
err_code_t fs_read_dir(struct fat_dir_entry_struct *dir_entry);
err_code_t fs_create_file(const char* file);
err_code_t fs_delete_file(const char* name);
err_code_t fs_rename_file(const char *name, const char* new_name);
uint32_t fs_file_size(const char *file);
err_code_t fs_reset_dir();
err_code_t fs_find_file(const char* file, struct fat_dir_entry_struct *dir_entry);
#endif /* FILESYS_H_ */