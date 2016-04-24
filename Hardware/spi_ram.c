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
 * SPI_RAM low level driver
 */
#include "spi_ram.h"

// définie le mode d'opération de la RAM
void sram_set_mode(uint8_t mode){
    _sram_select();
    spi_write(SRAM_WRMR);
    spi_write(mode);
    _sram_unselect();
}//f()

uint8_t sram_get_mode(){
    uint8_t mode;

    _sram_select();
    spi_write(SRAM_RDMR);
    mode=spi_write(0);
    _sram_unselect();
    return mode;
}//f()


void sram_send_address(uint16_t address){
    spi_write((uint8_t)(address>>8));
    spi_write((uint8_t)(address&0xff));
}//f()

// écris un octet dans RAM
void sram_write_byte(uint16_t address, uint8_t byte){
    _sram_select();
    spi_write(SRAM_WRITE);
    sram_send_address(address);
    spi_write(byte);
    _sram_unselect();
}//f()

// écris un mot de 16 bits
void sram_write_word(uint16_t address, uint16_t word){
    _sram_select();
    spi_write(SRAM_WRITE);
    sram_send_address(address);
    spi_write(word&0xff);
    spi_write(word>>8);
    _sram_unselect();
}//f()

// lit un octet de la RAM
uint8_t sram_read_byte(uint16_t address){
    uint8_t b;

    _sram_select();
    spi_write(SRAM_READ);
    sram_send_address(address);
    b=spi_write(0xff);
    _sram_unselect();
    return b;
}//f()

// lit un mot de 16 bits
uint16_t sram_read_word(uint16_t address){
    uint16_t word;

    _sram_select();
    spi_write(SRAM_READ);
    sram_send_address(address);
    word=spi_write(0xff);
    word|= spi_write(0xff)<<8;
    _sram_unselect();
    return word;
}//f()


//écris un bloc d'octets dans la RAM
void sram_write_block(uint16_t address, const uint8_t *data, uint16_t count){
    uint16_t i,b;

    if (!count) return;
    _sram_select();
    spi_write(SRAM_WRITE);
    sram_send_address(address);
    for (i=0;i<count;i++){
//        spi_write(data[i]);
        SPI_TX_BUF=data[i];
        while (!_spi_rxbuff_full());
        b=SPI_RX_BUF;
    }
    _sram_unselect();
}//f()

//lit un bloc d'octets de la RAM
void sram_read_block(uint16_t address, uint8_t *data, uint16_t count){
    uint16_t i;
    
    if (!count) return;
    _sram_select();
    spi_write(SRAM_READ);
    sram_send_address(address);
    for (i=0;i<count;i++){
//        data[i]=spi_write(0);
        SPI_TX_BUF=0;
        while(!_spi_rxbuff_full());
        data[i]=SPI_RX_BUF;
    }
    _sram_unselect();
}//f()

// remet à zéro tout le contenu de la RAM
void sram_clear(){
    uint32_t i;

    _sram_select();
    spi_write(SRAM_WRITE);
    sram_send_address(0);
    for (i=0;i<SRAM_SIZE;i++){
        spi_write(0);
    }
    _sram_unselect();
}//f()

void sram_clear_block(uint16_t address, uint16_t size){
    uint16_t i;
    
    if (!size) return;
    _sram_select();
    spi_write(SRAM_WRITE);
    sram_send_address(address);
    for (i=0;i<size;i++){
        spi_write(0);
    }
    _sram_unselect();
}
