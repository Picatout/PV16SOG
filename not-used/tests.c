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
 *  test unit
 */
#ifdef INCLUDE_TEST
#include <string.h>
#include <stdlib.h>
//#include <math.h>

#include "Hardware/hardware.h"
#include "Hardware/TVout.h"
#include "Hardware/systicks.h"
#include "graphics.h"
#include "text.h"
#include "Hardware/spi_ram.h"
#include "Hardware/joystick.h"
#include "Hardware/ps2_kbd.h"
#include "Hardware/tones.h"
#include "filesys.h"
#include "editor.h"
#include "Hardware/tones.h"
#include "Hardware/ntsc_const.h"
#include "Hardware/systicks.h"

#define DBG_FIRST 65000

static unsigned save_addr=DBG_FIRST;

// aide au débogage
void debug_save(int val){
    sram_write_word(save_addr,val);
    save_addr+=2;
    sram_write_word(save_addr,0xFFFF);
}

//imprime la trace
//sauvegardée par debug_save())
void debug_print(){
    unsigned addr;
    unsigned val;
    
    addr=DBG_FIRST;
    new_line();
    do{
        val=sram_read_word(addr);
        addr+=2;
        print_hex(val,6);
    }while ((addr<65535) && (val<0xFFFF));
    new_line();
    prompt("any...","*");
    save_addr=DBG_FIRST;
    sram_write_word(save_addr,0xFFFF);
}


// test son

//joue les 16 notes de la plus basse
// à la plus haute.
void scale_up(){
    uint16_t i;
    for (i=0;i<16;i++) scale_tone(i,50,true);
}


// check put_char() speed
void char_per_second(){
    uint16_t count=0;
    char c;
    
    cls();
    game_timer=1000;
    while(game_timer){
       c=random()%96+32;
       put_char(c);
       if (game_timer>1000){
            print_hex(game_timer,6);
            while(1);
       }
        count++;
    }
    cls();
    print("char per second: ");
    print_int(count,0);
    new_line();
    pause(2000);
}

// lines
void random_lines(){
    uint16_t x1, y1, x2, y2;
    uint8_t color;
    int count=0;
    game_timer=1000;
    while (game_timer){
        x1=random()%HRES;
        y1=random()%VRES;
        x2=random()%HRES;
        y2=random()%VRES;
        color=random()&15;
        draw_line(x1,y1,x2,y2,color);
        count++;
    }
    cls();
    print("lines/sec:" );
    print_int(count,4);
    pause(1000);
}//f()


void cls_speed(){
    unsigned t0,count;

    count=0;
    t0=get_ticks()+1000;
    while (1){
        cls();
        count++;
        if (get_ticks()>=t0)break;
    }
    print("cls()/sec: ");
    print_int(count,6);
    pause(2000);

}//f()

void sram_test(){
    while (1){
        gray_scale();
        print("saving video_buffer\n");
        sram_write_block(0,TV_BUFFER,(const uint8_t*)video_buffer);
        cls();
        prompt("waiting key to restore screen."," ");
        sram_read_block(0,TV_BUFFER,(uint8_t*)video_buffer);
        prompt("screen should be back\n"," ");
    }
}//f()

void joystick_test(){
    uint8_t btns;
    int x=HRES>>1,y=VRES>>1;

    cls();
    while (1){
        btns=joystick_read();
        switch (btns){
        case BTN_UP:
            if (y>0) y--;
            break;
        case BTN_DOWN:
            if (y<(VRES-1)) y++;
            break;
        case BTN_LEFT:
            if (x>0) x--;
            break;
        case BTN_RIGHT:
            if (x<(HRES-1)) x++;
            break;
        case BTN_UP+BTN_RIGHT:
            if (y>0) y--;
            if (x<(HRES-1)) x++;
            break;
        case BTN_RIGHT+BTN_DOWN:
            if (x<(HRES-1)) x++;
            if (y<(VRES-1)) y++;
            break;
        case BTN_DOWN+BTN_LEFT:
            if (y<(VRES-1)) y++;
            if (x>0) x--;
            break;
        case BTN_LEFT+BTN_UP:
            if (x>0) x--;
            if (y>0) y--;
            break;
        case BTN_FIRE:
            cls();
            x=HRES>>1;
            y=VRES>>1;
            break;
        }//switch()
        draw_pixel(x,y,7);
        pause(20);
    }//while
}//f()

void pixels_test(){
    int x,y;
    
    cls();
    for (y=0;y<VRES;y++){
        for (x=0;x<HRES/2;x+=2){
            draw_pixel(x,y,15);
        }
        for (x=HRES/2+1;x<HRES;x+=2){
            draw_pixel(x,y,15);
        }
    }
    while(1);
}//f()

void kbd_test(){
    uint16_t code;

    cls();
    if (!kbd_reset()) print("echec reset\n");
    while (!kbd_queue_empty()){
        print_hex(kbd_get_key(),4);
        print("\n");
    }
    while (1){
        code=kbd_get_key();
        if (code){
            put_char(code&127);
            //new_line();
        }
    }
}//f()

#define ENTRY_SIZE 32
static int16_t list_files(){
    uint8_t fname[ENTRY_SIZE];
    int16_t entry_nbr;
    struct fat_dir_entry_struct dir_entry;

   /* charge les fichiers dans la SRAM */
    entry_nbr=0;
    fname[ENTRY_SIZE-1]=0;
    while(fs_read_dir(&dir_entry))
    {
        if (!(dir_entry.attributes & FAT_ATTRIB_DIR)){
            print(dir_entry.long_name);
            print(" ");
            print_int(dir_entry.file_size,0);
            new_line();
            entry_nbr++;
        }
    }
    return entry_nbr;
}

void sdc_test(){
    unsigned fcount;
    char fname[ENTRY_SIZE];
//    struct fat_dir_entry_struct dir_entry;
    struct fat_file_struct *fh;
    
    cls();
    print("test 1 liste fichiers\n");
    fcount=list_files();
    print("nombre de fichiers: ");
    print_int(fcount,0);
    new_line();
    // test création d'un fichier
    print("test 2 creation fichier\n");
    if (!fs_create_file("test3.txt")){
        prompt("echec creation\n","");
    }else{
        print("succes creation\n");
        if ((fh=fs_open_file("test3.txt"))){
            print("fichier ouvert\n");
            fs_write_file(fh,(const uint8_t*)"hello world!12345678",20);
            fs_write_file(fh,(const uint8_t*)"bonjour chez vous12",20);
            fs_close_file(fh);
            fh=fs_open_file("test3.txt");
            fs_read_file(fh,(uint8_t*)&fname,ENTRY_SIZE);
            fname[14]=0;
            print(fname);
            fs_close_file(fh);
        }
    }
    print("btn -> quitter\n");
    while(!(joystick_any_btn()));
    while(joystick_any_btn());
}//f()


void img_test(const char* name, int lx, int ty){
    unsigned int left,count,i,x,y,width,height;
    uint8_t buffer[32];
    uint16_t addr=0;
    left=fs_load_file(name,0,SRAM_SIZE-1);
    cls();
    if (left){
        count=min(32,left);
        sram_read_block(addr,count,buffer);
        addr+=count;
        left-=count;
        width=buffer[0];
        height=buffer[1]; 
        i=2;
        for (y=ty;y<min(VRES,height+ty);y++){
            for (x=lx;x<min(HRES,width+lx);x++){
                draw_pixel(x,y,buffer[i++]);
                if (i==32){
                    count=min(32,left);
                    sram_read_block(addr,count,buffer);
                    addr+=count;
                    left-=count;
                    i=0;
                }
            }
        }
    }//if
    while(1);
}//f()

const uint8_t sprite[]={ //sprite 8x8
0x11,0x1e,0xe1,0x11,
0x11,0xe1,0x1e,0x11,
0x1e,0x11,0x11,0xe1,
0xe1,0x17,0x17,0x1e,
0xee,0x11,0x11,0xe1,
0x11,0xee,0xee,0x11,
0x11,0xe1,0x1e,0x11,
0x1e,0x11,0x11,0xe1
};

void sprite_test(){
    int x=0,y=0,dx=1,dy=1;
    bool quit=false;
    set_back_color(cDARK_GRAY);
    cls();
    print("sprite test");
    while (!quit){
        draw_sprite(x,y,8,8,sprite);
        pause(100);
        draw_sprite(x,y,8,8,sprite);
        x+=dx;
        if ((x<0) || (x>=HRES-7)){
            beep(100,20,false);
            dx=-dx;
        }
        y+=dy;
        if ((y<0) || (y>=VRES-7)){
            beep(100,20,false);
            dy=-dy;
        }
        switch(joystick_read()){
            case BTN_FIRE:
                quit=1;
                break;
            case BTN_UP:
                dy=-1;
                dx=0;
                break;
            case BTN_DOWN:
                dy=1;
                dx=0;
                break;
            case BTN_LEFT:
                dx=-1;
                dy=0;
                break;
            case BTN_RIGHT:
                dx=1;
                dy=0;
                break;                      ;
            case BTN_UP|BTN_RIGHT:
                dx=1;
                dy=-1;
                break;
            case BTN_UP|BTN_LEFT:
                dx=-1;
                dy=-1;
                break;
            case BTN_DOWN|BTN_LEFT:
                dx=-1;
                dy=1;
                break;
            case BTN_DOWN|BTN_RIGHT:
                dy=1;
                dx=1;
                break;

        }
    }
}//f()

#endif //INCLUDE_TEST