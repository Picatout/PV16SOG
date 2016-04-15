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
 * filesys.c
 *
 * Created: 2014-10-11 21:08:50
 *  Author: Jacques Deschênes
 *  une seule partition ouverte
 *  un seul système de fichier ouvert
 *  un seul répertoire ouvert.
 *  un seul fichier ouvert.
 */ 

#include <string.h>
#include <stdlib.h>

#include "filesys.h"
#include "text.h"
#include "Hardware/spi_ram.h"
#include "errors.h"



static struct fat_fs_struct *fs;
static struct fat_dir_struct *dd;
static struct partition_struct *partition;

static err_code_t fs_error=eERR_NONE;

err_code_t fs_last_error(){
    err_code_t error;
    
    error=fs_error;
    fs_error=eERR_NONE;
    return error;
}

err_code_t fs_mount(){
    if (get_pin_available()){
        // pas de carte SD
        // dans le lecteur
        fs_error=eERR_SDC;
        return fs_error;
    }
    if (partition) fs_umount();
    /* ouvre la première partition */
    partition = partition_open(sd_raw_read, sd_raw_read_interval, sd_raw_write, sd_raw_write_interval, 0);
    if(!partition)
    {
        /* Si la partition n'a pas ouvert essaie
         * pour  "superfloppy", i.e. pas de MBR.
         */
        partition = partition_open(sd_raw_read, sd_raw_read_interval, sd_raw_write, sd_raw_write_interval, -1);
        if(!partition)
        {   // rien à faire avec cette carte
            fs_error=eERR_PARTITION;
            return fs_error;
        }
    }

    /* ouvre le système de fichiers */
    fs = fat_open(partition);
    if(!fs)
    {
        partition_close(partition);
        fs_error= eERR_FAT;
        return fs_error;
    }
    fs_error=eERR_NONE;
    return fs_error;
}//f()

void fs_umount(){
    fat_close_all_files();
    fat_close_dir(dd);
    fat_close(fs);
    partition_close(partition);
}//f()


err_code_t  fs_open_dir(const char *dir_name){
    /* ouvre le répertoire dir_name */
    struct fat_dir_entry_struct directory;

    fs_error=eERR_NONE;
    if (dd) fs_close_dir();
    fat_get_dir_entry_of_path(fs, dir_name, &directory);
    dd = fat_open_dir(fs, &directory);
    if(!dd)
    {
        fs_umount();
        fs_error=eERR_DIRECTORY;
    }
    return fs_error;
}//f()

void fs_close_dir(){
    fat_close_dir(dd);
}//f()

struct fat_file_struct *fs_open_file(const char *file_name){
    bool found=false;
    struct fat_file_struct *fh=NULL;
    struct fat_dir_entry_struct dir_entry;
    char name[32];
    
    fs_error=eERR_NONE;
    fat_reset_dir(dd);
    while(fat_read_dir(dd, &dir_entry))
    {
        strcpy(name,dir_entry.long_name);
        uppercase(name);
        if(strcmp(name, file_name) == 0)
        {
            fat_reset_dir(dd);
            found= 1;
            break;
        }
    }

    if (!found){ 
        fs_error=eERR_NOTFILE;
    }else if(!(fh=fat_open_file(fs, &dir_entry))){
        fs_error=eERR_OPENFAIL;
    }
    return fh;
}//f()


err_code_t fs_file_reset(struct fat_file_struct *fh){
    fs_error=eERR_NONE;
    if (!fat_seek_file(fh,0,FAT_SEEK_SET)) fs_error=eERR_SEEK;
    return fs_error;
}//f()

uint16_t fs_read_file(struct fat_file_struct *fh, uint8_t* buffer, uint16_t buffer_len){
    int16_t count;
    
    fs_error=eERR_NONE;
    if ((count=fat_read_file(fh,buffer,buffer_len))==-1)fs_error=eERR_FILEREAD;
    return count;
}//f()

uint16_t fs_write_file(struct fat_file_struct *fh,const uint8_t* buffer, uint16_t buffer_len){
    int16_t count;
    
    fs_error=eERR_NONE;
    if((count=fat_write_file(fh, buffer, buffer_len))==-1)fs_error=eERR_FILEWRITE;
    return count; 
}//f()

// charge le fichier dans la SPIRAM
// retourne nombres d'octets
uint16_t fs_load_spiram(const char *file,uint16_t sram_address, uint16_t maxsize){
#define BUFF_SIZE (64)
    uint16_t byte_count=0;
    int count;
    uint8_t buffer[BUFF_SIZE];
    struct fat_file_struct *fh;

    uppercase(file);
    fs_error=eERR_NONE;
    if ((fh=fs_open_file(file))){
        while((count = fat_read_file(fh,buffer, BUFF_SIZE))>-1){
            if (!count){break;}
            byte_count+=count;
            if (byte_count>maxsize) {fs_error=eERR_FILE2BIG; byte_count=0; break;}
            sram_write_block(sram_address,(const uint8_t*)buffer,count);
            sram_address+=count;
        }
        if (byte_count) sram_write_byte(sram_address,0);
        fat_close_file(fh);
    }else{
        fs_error=eERR_OPENFAIL;
    }
    return byte_count;
}//f()

// sauvegarde la RAM SPI dans un fichier.
void fs_save_spiram(const char *file,uint16_t sram_address, uint16_t size){
#define BUFF_SIZE (64)
    uint8_t buffer[BUFF_SIZE];
    uint16_t count;
    struct fat_file_struct *fh;
    intptr_t result;
    fs_error=eERR_NONE;
    uppercase(file);
    fs_create_file(file);
    if (fs_error==eERR_NONE && (fh=fs_open_file(file))){
        while(fs_error==eERR_NONE && size){
            count=min(size,BUFF_SIZE);
            sram_read_block(sram_address,buffer,count);
            size-=count;
            sram_address+=count;
            result=fat_write_file(fh,buffer,count);
            if (result!=count){
                fs_error=eERR_FILEWRITE;
                break;
            }
        }
        fat_close_file(fh);
    }else{
        fs_error=eERR_OPENFAIL;
    }
}//f

err_code_t fs_read_dir(struct fat_dir_entry_struct *dir_entry){
    fs_error=eERR_NONE;
    if (!fat_read_dir(dd, dir_entry))fs_error=eERR_DIRECTORY;
    return fs_error;
}//f()

err_code_t fs_create_file(const char* file){
    struct fat_dir_entry_struct dir_entry;
    fs_error=eERR_NONE;
    if (!fat_create_file(dd,file,&dir_entry))fs_error=eERR_FCREATE;
    return fs_error;
}//f()

err_code_t fs_delete_file(const char *name){
    struct fat_dir_entry_struct dir_entry;
    
    if (!(fs_error=fs_find_file(name,&dir_entry))){
        if (!fat_delete_file(fs,&dir_entry))fs_error=eERR_FILEWRITE;
    }
    return fs_error;
}//f()

err_code_t fs_rename_file(const char *name, const char* new_name){
    struct fat_dir_entry_struct  dir_entry;
    
    if (!(fs_error=fs_find_file(name,&dir_entry))){
        if (!fat_move_file(fs,&dir_entry,dd,new_name))fs_error=eERR_FILEWRITE;
    }
    return fs_error;
}//f()

uint32_t fs_file_size(const char *file){
    uint8_t success;
    uint32_t size=0;
    struct fat_dir_entry_struct dir_entry;
    char name[32];
    
    fs_error=eERR_NONE;
    if (fat_reset_dir(dd)){
        do {
            success=fat_read_dir(dd,&dir_entry);
            if (success && !(dir_entry.attributes&(FAT_ATTRIB_DIR|FAT_ATTRIB_VOLUME|FAT_ATTRIB_SYSTEM))){
                strcpy(name,dir_entry.long_name); 
                uppercase(name);
                if(strcmp(file,name)==0){
                    size=dir_entry.file_size;
                    break;
                }
            }
        } while (success);
    }else{
        fs_error=eERR_DIRECTORY;
    }
    return size;
}

err_code_t fs_reset_dir(){
    fs_error=eERR_NONE;
    if (!fat_reset_dir(dd)) fs_error=eERR_DIRECTORY;
    return fs_error;
}//f()

err_code_t fs_find_file(const char* file, struct fat_dir_entry_struct *dir_entry){
    char name[32];
    fs_error=fs_reset_dir();

    if (!fs_error){
        do {
            fs_error=fs_read_dir(dir_entry);
            if (!fs_error && !(dir_entry->attributes&(FAT_ATTRIB_DIR|FAT_ATTRIB_VOLUME|FAT_ATTRIB_SYSTEM))){
                strcpy(name,dir_entry->long_name);
                uppercase(name);
               if (strcmp(file,name)==0){
                    break;
                }
            }
        }while (!fs_error);
        if (fs_error) fs_error=eERR_NOTFILE;
    }
    return fs_error;
}//f()

