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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

//#include "Hardware/hardware.h"
#include "text.h"
#include "Hardware/TVout.h"
#include "Hardware/spi_ram.h"
#include "Hardware/ps2_kbd.h"
#include "filesys.h"
#include "Hardware/tones.h"
#include "graphics.h"
#include "font6x8.h"
#include "reader.h"
#include "stackvm.h"

#define LAST_COL (CHAR_PER_LINE-1)
#define LINE_MAX_LEN (CHAR_PER_LINE-1)
#define LAST_LINE (LINE_PER_SCREEN-1)
#define SCREEN_SIZE (LINE_PER_SCREEN*CHAR_PER_LINE)
#define ED_BUFF_SIZE (SRAM_SIZE-2)
#define MAX_SIZE (ED_BUFF_SIZE)

#define refused()  beep(500,100,false);

#define MODE_INSERT 1
#define MODE_OVERWR 0
#define NAME_MAX_LEN 32

//contient 1'�cran texte
uint8_t screen[LINE_PER_SCREEN][CHAR_PER_LINE];

typedef struct editor_state{
    uint16_t fsize; // grandeur du fichier en octets.
    uint16_t scr_first; //premier caract�re visible
    uint16_t gap_first; //position d�but gap
    uint16_t tail; // position fin gap
    int8_t scr_line; // ligne du curseur
    int8_t scr_col; //colonne du curseur
    struct {
      uint8_t insert:1;
      uint8_t modified:1;
      uint8_t new:1;
    }flags;
} ed_state_t;


//static char *split_buffer; // ligne en edition
static ed_state_t state;
//static uint8_t *llen;
static char fname[32];

typedef struct search_struct{
   bool ignore_case;
   bool loop;
   bool found;
   uint8_t col;
   char target[CHAR_PER_LINE];
} search_t;

static search_t search_info;

extern void cmd_dir();

static void open_file();
static void load_file(const char* name);
static void save_file(const char* name);
static void save_file_as();
static void line_up();
static void line_down();
static void char_left();
static void char_right();
static void delete_left();
static void delete_at();
static void delete_to_end();
static void delete_to_start();
static void insert_char(char c);
static void replace_char(char c);
static void line_home();
static void line_end();
static void enter();
static void file_home();
static void file_end();
static void update_display();
static void page_up();
static void page_down();
static void word_right();
static void word_left();
static void goto_line();
static uint8_t get_line_forward(uint8_t* line, uint16_t from);
static uint8_t get_line_back(uint8_t* line, uint16_t from);
static void jump_to_line(uint16_t line_no);

static void editor_colors(){
    set_back_color(0);
    set_font_color(12);
}

static void info_colors(){
    set_back_color(4);
    set_font_color(15);
}

static void print_line(uint8_t line){
    text_clear_line(line);
    set_cursor(0,line);
    print((char*)&screen[line]);
}

//si fichier modifi� confirme 
//avant de continuer
static bool ask_confirm(){
    char key;
    bool answer=state.flags.modified;
    if (answer){
        beep(1000,100,false);
        info_colors();
        set_cursor(0,0);
        clreol();
        print("file unsaved! continue (y/n)?");
        key= wait_key();
        answer=(key=='y')||(key=='Y');
        editor_colors();
        set_cursor(0,0);
        clreol();
        print_line(0);
        set_cursor(state.scr_col,state.scr_line);
    }else{
        answer=true;
    }
    return answer;
}

static void new_file(){
    if (ask_confirm()){
        info_colors();
        cls();
        sram_clear_block(0,ED_BUFF_SIZE);
        memset(&state,0,sizeof(ed_state_t));
        memset(screen,0,SCREEN_SIZE);
        state.flags.insert=MODE_INSERT;
        state.flags.new=1;
        state.flags.modified=false;
        state.tail=MAX_SIZE;
        cls();
        fname[0]=0;
        search_info.target[0]=0;
        search_info.ignore_case=false;
        search_info.loop=false;
        search_info.found=false;
        editor_colors();
        update_display();
    }
}//f()

static bool quit;

static void leave_editor(){
    if (ask_confirm()){
        set_cursor_shape(cVLINE);
        quit=true;
    }
}

extern void cmd_dir();
static void list_files(){
    info_colors();
    cls();
    cmd_dir();
    prompt_continue();
    editor_colors();
    update_display();
}//f

//initialement le curseur est au d�but de la ligne
static void mark_target(){
    int i,len;

    len=strlen(search_info.target);
    set_cursor(state.scr_col,state.scr_line);
    set_back_color(7);
    for (i=0;i<len;i++) put_char(screen[state.scr_line][state.scr_col+i]);
    set_back_color(0);
    set_cursor(state.scr_col,state.scr_line);
}//f

static int search_line(char *line, char *target, int from){
    int i,j=0;
    for (i=from;line[i];i++){
        if (search_info.target[j]==line[i]){
            j++;
            if (j==strlen(search_info.target)){
                i++;
                break;
            }
        }else{
             j=0; 
        }
    }
    return j==strlen(search_info.target)?i-j:-1;
}//f

static void search_next(){
    char line[CHAR_PER_LINE];
    int len,lcount=0,pos;
    uint16_t saddr;
    
    if (!search_info.target[0] || (!search_info.loop && (state.gap_first>=(state.fsize-1)))){
        refused();
        return;
    }
    search_info.found=false;
    strcpy(line,(char*)&screen[state.scr_line]);
    if (search_info.ignore_case) uppercase(line);
    pos=search_line(line,search_info.target,state.scr_col+1);
    if (pos>state.scr_col){
        search_info.col=pos;
        state.scr_col=pos;
        mark_target();
        return;
    }
    len=strlen((char*)&screen[state.scr_line]);
    saddr=state.tail+(len-state.scr_col)+1;
    while (saddr<ED_BUFF_SIZE){
        lcount++;
        len=get_line_forward((uint8_t*)line,saddr);
        if (search_info.ignore_case) uppercase(line);
        pos=search_line(line,search_info.target,0);
        if (pos>-1){
            search_info.found=true;
            search_info.col=(uint8_t)pos;
            break;
        }
        saddr+=len+1;
    }//while
    if (search_info.found){
        pos=state.scr_line+lcount;
        line_down();
        saddr=state.tail;
        state.scr_col=search_info.col;
        while (lcount){
            len=get_line_forward((uint8_t*)line,saddr);
            lcount--;
            if (lcount){
                sram_write_block(state.gap_first,(uint8_t*)line,len+1);
                state.gap_first+=len+1;
                sram_clear_block(state.tail,len+1);
                state.tail+=len+1;
                saddr=state.tail;
            }else{
                sram_write_block(state.gap_first,(uint8_t*)line,search_info.col);
                state.gap_first+=search_info.col;
                sram_clear_block(state.tail,search_info.col);
                state.tail+=search_info.col;
            }
        }//while
        if (pos>LAST_LINE){
            state.scr_first=state.gap_first-search_info.col;
            state.scr_line=0;
            update_display();
        }else{
            state.scr_line=pos;
            set_cursor(state.scr_col,state.scr_line);
        }
        mark_target();
    }else if (search_info.loop){
        saddr=0;
        lcount=1;
        while (saddr<state.gap_first){
            len=get_line_forward((uint8_t*)line,saddr);
            if (search_info.ignore_case) uppercase(line);
            pos=search_line(line,search_info.target,0);
            if (pos>-1){
                search_info.found=true;
                search_info.col=(uint8_t)pos;
                break;
            }
            lcount++;
            saddr+=len+1;
        }//while
        if (search_info.found){
            jump_to_line(lcount);
            while (state.scr_col<search_info.col) char_right();
            mark_target();
        }else{
            refused();
        }
    }else{
        refused();
    }
}//f

static bool parse_search_line(){
    int pos=0;
    char *str, prev=0;
    int options=0;
    
    str=search_info.target;
    while (str[pos]){
        if (str[pos]=='-' && prev==' '){
            str[pos-1]=0;
            prev='-';
            pos++;
            break;
        }
        prev=str[pos];
        pos++;
    }    
    while (str[pos] && options!=3){
        switch(str[pos]){
            case '-':
            case ' ':
                break;
            case 'l':
            case 'L':
                if (prev!='-'){
                    refused();
                    return false;
                }
                search_info.loop=true;
                options|=1; 
                if (str[pos+1] && str[pos+1]!=' '){
                    refused();
                    return false;
                }
                break;
            case 'i':
            case 'I':
                if (prev!='-'){
                    refused();
                    return false;
                }
                search_info.ignore_case=true;
                options|=2;
                uppercase(str);
                if (str[pos+1] && str[pos+1]!=' '){
                    refused();
                    return false;
                }
                break;
            default:
                refused();
                return false;
        }//switch
        prev=str[pos];
        pos++;
    }//while
    return true;
}//f

static void search(){
    int len;
    info_colors();
    cls();
    print("USAGE: target [-I] [-L]\n");
    search_info.found=false;
    search_info.loop=false;
    search_info.ignore_case=false;
    print("? ");
    len=readln(search_info.target,CHAR_PER_LINE);
    if (search_info.target[len-1]=='\n'){
        search_info.target[len-1]=0;
        len--;
    }
    editor_colors();
    update_display();
    if (len && parse_search_line()){
        search_next();
    }
}//f

static bool get_file_name(char *name){
    int len;
    
    new_line();
    print("file name? ");
    len=readln(name,NAME_MAX_LEN);
    if (name[len-1]=='\n')name[len-1]=0;
    uppercase(name);
    return len;
}//f()

static void load_file(const char* name){
    uint32_t fsize;
    int count, line_no=0;
    uint16_t saddr;
    char c=0, buffer[CHAR_PER_LINE];
    err_code_t error_code=eERR_NONE;
    struct fat_file_struct *fh;
    reader_t r;
    
    new_file();
    fsize=fs_file_size(name);
    if ((!fsize) || (error_code=fs_last_error())){
        error(error_code);
        return;
    }
    if (fsize>MAX_SIZE){
        error(eERR_FILE2BIG);
        return;
    }
    if (!(fh=fs_open_file(name))){
        error(fs_last_error());
        return;
    }
    count=0;
    state.tail=MAX_SIZE;
    saddr=0;
    print("loading file...\n");
    print_int(fsize,0);
    print(" bytes");
    reader_init(&r,eDEV_SDCARD,fh);
    while (!r.eof){
        c=reader_getc(&r);
        switch(c){
            case -1:
            case '\r':
            case '\n':
                break;
            default:
                if (c>=32 && c<(32+FONT_SIZE)) buffer[count++]=c; else buffer[count++]=' ';
        }
        if ((c=='\n') || (count==(CHAR_PER_LINE-1))){
            buffer[count++]=0;
            line_no++;
            sram_write_block(saddr,(uint8_t*)buffer,count);
            saddr+=count;
            count=0;
        }
    }
    if (count){
        sram_write_block(saddr,(uint8_t*)buffer,count);
        saddr+=count;
        line_no++;
    }
    fs_close_file(fh);
    strcpy(fname,name);
    editor_colors();
    state.fsize=saddr;
    state.gap_first=saddr;
    state.scr_first=saddr;
    file_home();
}//f()

static void open_file(){
    struct fat_dir_entry_struct dir_entry;
    char name[32];

    if (ask_confirm()){
        info_colors();
        cls();    
        print("open file\n");
        if (get_file_name(name)){
            if (!fs_find_file(name,&dir_entry)){
                load_file(name);
                return;
            }else{
                error(fs_last_error());
            }
        }
        editor_colors();
        cls();
        update_display();
    }
}//f()

inline static void replace_nulls(uint8_t *buffer,int len){
    int i;
    for(i=0;i<=len;i++) if (!buffer[i]) buffer[i]='\n';
}//f

static void save_file(const char* name){
#define BUFFER_SIZE 128
    uint8_t buffer[BUFFER_SIZE];
    int size;
    uint16_t saddr=0;
    err_code_t err;
    struct fat_file_struct *fh;
    struct fat_dir_entry_struct dir_entry;

    if (fs_find_file(name,&dir_entry)==eERR_NONE){
        fs_delete_file(name);
    }
    if ((err=fs_create_file(name))){
        error(err);
        return;
    }
    if ((fh=fs_open_file(name))){
        info_colors();
        cls();
        print("saving file...\n");
        while (saddr < state.gap_first){
            size=min(BUFFER_SIZE,state.gap_first-saddr);
            sram_read_block(saddr,buffer,size);
            saddr+=size;
            replace_nulls(buffer,size);
            fs_write_file(fh,buffer,size);
        }
        saddr=state.tail;
        while(saddr < ED_BUFF_SIZE){
            size=min(BUFFER_SIZE,ED_BUFF_SIZE-saddr);
            sram_read_block(saddr,buffer,size);
            saddr+=size;
            replace_nulls(buffer,size);
            fs_write_file(fh,buffer,size);
        }
        fs_close_file(fh);
        state.flags.modified=false;
        state.flags.new=0;
        strcpy(fname,name);
        new_line();
        print(fname);
        print(" saved, size: ");
        print_int(state.fsize,0);
        prompt_continue();
        editor_colors();
        update_display();
    }else{
        error(fs_last_error());
    }
}//f()

static void save_file_as(){
    char name[32];
    info_colors();
    cls();
    if (get_file_name(name)){
        if (strlen(name))
            save_file(name);
    }
    editor_colors();
    update_display();
}//f()

static void file_info(){
    char line[CHAR_PER_LINE];
    set_cursor(0,0);
    info_colors();
    clreol();
    if (strlen(fname)){
        sprintf(line,"file%c: %s, size: %d",state.flags.modified?'*':' ',fname,state.fsize);
    }else{
        sprintf(line,"no name%c, size: %d",state.flags.modified?'*':' ',state.fsize);
    }
    print(line);
    prompt_continue();
    editor_colors();
    print_line(0);
    print_line(1);
    set_cursor(state.scr_col,state.scr_line);
}


__eds__ PSTR hkeys[]={
  "<CTRL-DEL> delete to end of line\n",
  "<CTRL-A> save as\n",
  "<CTRL-END> file end\n",
  "<CTRL-F> list SDcard files\n",
  "<CTRL-G> goto line...\n",
  "<CTRL-H> display hotkeys\n",
  "<CTRL-HOME> file start\n",
  "<CTRL_LEFT> word left\n",
  "<CTRL-N> new file...\n",
  "<CTRL-O> open file...\n",
  "<CTRL-Q> Quit editor\n",
  "<CTRL-RIGHT> word right\n",
  "<CTRL-S> save file\n",
  "<F1> file name and size\n"
  "<F3> set search criterion\n",
  "<SHIFT-F3> search next\n",
  ""
};

//affiche les touches d'action
static void hot_keys(){
    char text[64];
    int i=0;

    info_colors();
    cls();
    pstrcpy(text,hkeys[i++]);
    while (strlen(text)){    
        print(text);
        pstrcpy(text,hkeys[i++]);
    }
    new_line();
    prompt_continue();
    editor_colors();
    update_display();
}

static void editor_init(){
    editor_colors();
    set_cursor_shape(cVLINE);
    cls();
    fname[0]=0;
    new_file();
}//f()


void editor(const char* name){
    uint16_t key;
    struct fat_dir_entry_struct dir_entry;
     
    state.flags.modified=false;
    editor_init();
    if (name){
        if (!fs_find_file(name,&dir_entry)) 
            load_file(name);
        else if (fs_last_error()==eERR_NOTFILE){
            strcpy(fname,name);
        }
    }
    quit=false;
    while(!quit){
        key=wait_key();
        switch(key){
            case VK_UP:
                line_up();
                break;
            case VK_DOWN:
                line_down();
                break;
            case VK_LEFT:
                if (key_state & F_CTRL){
                    word_left();
                }else{
                    char_left();
                }
                break;
            case VK_RIGHT:
                if (key_state & F_CTRL){
                    word_right();
                }else{
                    char_right();
                }
                break;
            case VK_HOME:
                if ((key_state & F_CTRL))
                    file_home();
                else
                    line_home();
                break;
            case VK_END:
                if ((key_state & F_CTRL))
                    file_end();
                else
                    line_end();
                break;
            case  VK_PGUP:
                page_up();
                break;
            case VK_PGDN:
                page_down();
                break;
            case VK_INSERT:
                state.flags.insert=~state.flags.insert;
                if (state.flags.insert){
                    set_cursor_shape(cVLINE);
                }else{
                    set_cursor_shape(cBLOCK);
                }
                break;
            case VK_DELETE:
                if ((key_state & F_LCTRL)){
                    delete_to_end();
                }else{
                    delete_at();
                }
                break;
            case VK_BACK:
                if (key_state & F_LCTRL){
                    delete_to_start();
                }else{
                    delete_left();
                }
                break;
            case VK_ENTER:
                enter();
                break;
            case VK_F1:
                file_info();
                break;
            case VK_F3:
                if (key_state & F_SHIFT) search_next();else search();
                break;
            default:
                if (key_state & F_CTRL){
                    switch (key){
                        case 'a': // sauvegarde sous...
                            save_file_as();
                            break;
                        case 'f': // liste des fichiers
                            list_files();
                            break;
                        case 'g':// va � la ligne
                            goto_line();
                            break;
                        case 'h': // affiche hot keys
                            hot_keys();
                            break;
                        case 'n': // nouveau fichier
                            new_file();
                            break;
                        case 'o': // ouvrir un fichier
                            open_file();
                            break;
                        case 'q': // quitter
                            leave_editor();
                            break;
                        case 's': // sauvegarde
                            info_colors();
                            cls();
                            if (strlen(fname)) save_file(fname); else save_file_as();
                            editor_colors();
                            update_display();
                            break;
                        default:
                            refused();
                            
                    }//switch
                }else  if ((key>=32)&(key<FONT_SIZE+32)){
                    if (state.flags.insert){
                        insert_char(key);
                    }else{
                        replace_char(key);
                    }
                }
            }//switch
    }
    set_font_color(15);
    cls();
}//f()


// cherche ligne avant from
// condition: from doit point� le premier caract�re d'une ligne
// argurments:
//   line-> buffer recevant la ligne
//   from -> adresse SRAM d�but ligne courante
// retourne la longueur de la ligne
uint8_t get_line_back(uint8_t *line, uint16_t from){
    int j,size;

    if (from==0){
        line[0]=0;
        return 0;
    }
    if (from<=state.gap_first){
        size=min(CHAR_PER_LINE,from);
        sram_read_block(from-size,line+(CHAR_PER_LINE-size),size);
    }else{
        size=min(from-state.tail,CHAR_PER_LINE);
        sram_read_block(from-size,line+(CHAR_PER_LINE-size),size);
        if (size<CHAR_PER_LINE && state.gap_first>0){
            j=min(CHAR_PER_LINE-size,state.gap_first);
            size+=j;
            sram_read_block(state.gap_first-j,line+(CHAR_PER_LINE-size),j);
        }
    }
    if (size<CHAR_PER_LINE) line[CHAR_PER_LINE-size-1]=0;
    j=CHAR_PER_LINE-1;
    if (!line[j]){
        j--;
    }
    while (j>0 && line[j]) j--;
    if (!line[j]) strcpy((char*)line,(char*)&line[j+1]);
    return strlen((char*)line);
}//f()


//cherche ligne d�butant a partir de from
//condition: from pointe le premier caract�re de la ligne courante
// arguments:
//   from -> adresse SRAM d�but de ligne
//   line -> buffer recevant la ligne
// retourne la longueur de la ligne
uint8_t get_line_forward(uint8_t *line, uint16_t from){
    int j,size;
    
    if (from <= state.gap_first){
        size=min(CHAR_PER_LINE-1, state.gap_first-from);
        sram_read_block(from,line,size);
        if (size<(CHAR_PER_LINE-1) && state.tail<MAX_SIZE){
            from=state.tail;
            j=min(CHAR_PER_LINE-size-1,MAX_SIZE-from);
            sram_read_block(from,&line[size],j);
            size+=j;
        }
    }else{
        size=min(CHAR_PER_LINE-1,MAX_SIZE-from);
        sram_read_block(from,line,size);
    }
    line[size]=0;
    size=strlen((char*)line);
    j=size;
    while (j<CHAR_PER_LINE) line[j++]=0;
    return size;
}//f()


static void update_display(){
    uint16_t from;
    uint8_t scr_line=0, llen=0;
 
    _disable_video();
    cls();
    memset(screen,0,SCREEN_SIZE);
    from=state.scr_first;
    while (from<MAX_SIZE && scr_line<LINE_PER_SCREEN){
        llen=get_line_forward((uint8_t*)&screen[scr_line],from);
        print_line(scr_line);
        scr_line++;
        if (((from<state.gap_first) && ((from+llen+1)<state.gap_first))||
                (from>=state.tail)){
            from+=llen+1;
        }else{
            from=state.tail+((from+llen+1)-state.gap_first);
        }
    }
    set_cursor(state.scr_col,state.scr_line);
    _enable_video();
}//f();


static void line_up(){
    uint8_t llen, line[CHAR_PER_LINE];
    
    line_home();
    if (state.gap_first){ 
        llen=get_line_back(line,state.gap_first);
        state.gap_first-=llen+1;
        sram_clear_block(state.gap_first,llen);
        state.tail-=llen+1;
        sram_write_block(state.tail,line,llen+1);
        state.scr_col=0;
        if (state.gap_first < state.scr_first){
            state.scr_first=state.gap_first;
            scroll_down(CHAR_HEIGHT);
            memmove(&screen[1],screen, SCREEN_SIZE-CHAR_PER_LINE);
            memmove(screen,line,CHAR_PER_LINE);
            print_line(0);
        }else{
            state.scr_line--;
            memmove(&screen[state.scr_line],line,CHAR_PER_LINE);
            print_line(state.scr_line);
            
        }
        set_cursor(state.scr_col,state.scr_line);
    }
}//f()

static void line_down(){
    uint8_t llen,line[CHAR_PER_LINE];
    
    line_end();
    if (state.tail==MAX_SIZE){
        return;
    }
    sram_write_byte(state.tail++,0);
    sram_write_byte(state.gap_first++,0);
    state.scr_col=0;
    state.scr_line++;
    if (state.scr_line==LINE_PER_SCREEN){
        state.scr_line--;
        state.scr_first+=strlen((char*)&screen[0])+1;
        scroll_up(CHAR_HEIGHT);
        memmove(screen,(void*)&screen[1],SCREEN_SIZE-CHAR_PER_LINE);
        llen=get_line_forward(line,state.tail);
        strcpy((char*)&screen[LAST_LINE],(char*)line);
        print_line(LAST_LINE);
    }
    set_cursor(state.scr_col,state.scr_line);
}//f()

//d�place le curseur vers la gauche d'un caract�re
static void char_left(){
    if (state.scr_col){
        state.scr_col--;
        sram_write_byte(--state.tail,screen[state.scr_line][state.scr_col]);
        sram_write_byte(state.gap_first--,0);
        set_cursor(state.scr_col,state.scr_line);
    }else if (state.gap_first){
        line_up();
        line_end();
    }
}//f()

//d�place le curseur vers la droite d'un caract�re
static void char_right(){
    uint8_t c;
    
    if (state.gap_first==state.fsize) return;
    if (state.scr_col<strlen((char*)&screen[state.scr_line])){
        c=sram_read_byte(state.tail);
        sram_write_byte(state.gap_first++,c);
        sram_write_byte(state.tail++,0);
        state.scr_col++;
        set_cursor(state.scr_col,state.scr_line);
    }else{
        line_down();
    }
}//f()

static void delete_left(){
    if (state.gap_first){
        char_left();
        delete_at();
    }
} //f()

static void delete_at(){
//    int llen,i;
//    char c;
//    uint8_t line[CHAR_PER_LINE];
    
    if (state.tail==MAX_SIZE) return;
    sram_write_byte(state.tail++,0);
    state.fsize--;
    if (state.scr_col<strlen((char*)&screen[state.scr_line])){
        strcpy((char*)&screen[state.scr_line][state.scr_col],
                (char*)&screen[state.scr_line][state.scr_col+1]);
        print_line(state.scr_line);
        set_cursor(state.scr_col,state.scr_line);
    }else{
        update_display();
    }
}//f()

//efface tous les caract�res apr�s le cursor
//jusqu'� la fin de ligne.
//si le curseur est au d�but de ligne
//la ligne est supprim�e au complet '\n' inclus.
static void delete_to_end(){
    uint8_t llen,c;
    
    if (state.fsize==0) return;
    llen=strlen((char*)&screen[state.scr_line]);
    if (state.scr_col){
        memset(&screen[state.scr_line][state.scr_col],0,CHAR_PER_LINE-state.scr_col);
        c=llen-state.scr_col;
        sram_clear_block(state.tail,c);
        state.fsize-=c;
        state.tail+=c;
        print_line(state.scr_line);
    }else{
        memset(&screen[state.scr_line],0,CHAR_PER_LINE);
        sram_clear_block(state.tail,llen);
        state.tail+=llen;
        state.fsize-=llen;
        if (state.tail<ED_BUFF_SIZE){
            c=sram_read_byte(state.tail);
            if (c==0){
                sram_write_byte(state.tail,0);
                state.tail++;
                state.fsize--;
            }
        }
        update_display();
    }
    state.flags.modified=true;
}//f()

static void delete_to_start(){
    uint8_t buffer[CHAR_PER_LINE];
    
    if (state.scr_col){
        memset(buffer,0,CHAR_PER_LINE);
        state.gap_first-=state.scr_col;
        sram_write_block(state.gap_first,buffer,state.scr_col);
        memmove(&screen[state.scr_line],
                &screen[state.scr_line][state.scr_col],CHAR_PER_LINE-state.scr_col);
        state.fsize-=state.scr_col;
        state.scr_col=0;
        print_line(state.scr_line);
        state.flags.modified=true;
    }
}//f()

static void insert_char(char c){
    if (state.fsize==MAX_SIZE){
        refused();
        return;
    }
    if (strlen((char*)&screen[state.scr_line])<LINE_MAX_LEN){
        sram_write_byte(state.gap_first++,c);
        state.fsize++;
        memmove(&screen[state.scr_line][state.scr_col+1],
                &screen[state.scr_line][state.scr_col],CHAR_PER_LINE-state.scr_col-1);
        screen[state.scr_line][state.scr_col]=c;
        print_line(state.scr_line);
        state.scr_col++;
        set_cursor(state.scr_col,state.scr_line);
        state.flags.modified=true;
    }else{
        sram_write_byte(state.gap_first++,0);
        sram_write_byte(state.gap_first++,c);
        state.fsize+=2;
        state.scr_col=1;
        state.scr_line++;
        if (state.scr_line>LAST_LINE){
            state.scr_first+=strlen((char*)&screen[0])+1;
            state.scr_line=LAST_LINE;
        }
        state.flags.modified=true;
        update_display();
    }
}//f()

static void replace_char(char c){
    int llen;
    llen=strlen((char*)&screen[state.scr_line]);
    if (state.scr_col<llen){
        sram_write_byte(state.gap_first++,c);
        state.tail++;
        screen[state.scr_line][state.scr_col]=c;
        put_char(c);
        state.scr_col++;
        state.flags.modified=true;
        set_cursor(state.scr_col,state.scr_line);
    }else{
        insert_char(c);
    }
}//f();

static void line_home(){
    if (state.scr_col){
        state.tail-=state.scr_col;
        sram_write_block(state.tail,
                (uint8_t*)&screen[state.scr_line],state.scr_col);
        state.gap_first-=state.scr_col;
        sram_clear_block(state.gap_first,state.scr_col);
        state.scr_col=0;
        set_cursor(0,state.scr_line);
    }
}//f()

static void line_end(){
    uint8_t llen,forward;
    
    llen=strlen((char*)&screen[state.scr_line]);
    forward=llen-state.scr_col;
    if (forward){
        sram_write_block(state.gap_first,(uint8_t*)&screen[state.scr_line][state.scr_col],forward);
        state.gap_first+=forward;
        sram_clear_block(state.tail,forward);
        state.tail+=forward;
        state.scr_col=llen;
        set_cursor(state.scr_col,state.scr_line);
    }
}//f()

 static void enter(){
    if ((state.flags.insert) && (state.fsize==MAX_SIZE)){
        refused();
        return;
    }
    if (state.flags.insert){
        state.fsize++;
    }else{
        sram_write_byte(state.tail++,0);
    }
    sram_write_byte(state.gap_first++,0);
    state.scr_col=0;
    state.scr_line++;
    if (state.scr_line==LINE_PER_SCREEN){
        state.scr_first+=strlen((char*)&screen[0])+1;
        state.scr_line--;
    }
    update_display();
}//f()

static void file_home(){
    uint8_t size, maxlen, line[CHAR_PER_LINE];
    if (state.fsize==MAX_SIZE){
        state.scr_first=0;
        state.gap_first=0;
        state.tail=0;
        
    }else{
        maxlen=min(CHAR_PER_LINE,state.tail - state.gap_first);
        while (state.gap_first){
            size=min(maxlen,state.gap_first);
            state.gap_first-=size;
            sram_read_block(state.gap_first,line,size);
            sram_clear_block(state.gap_first,size);
            state.tail-=size;
            sram_write_block(state.tail,line,size);
        }
        state.scr_first=0;
    }
    state.scr_col=0;
    state.scr_line=0;
    update_display();
}//f()

static void file_end(){
    uint16_t saddr;
    int size=0, i;
    uint8_t maxlen, line[CHAR_PER_LINE];
    
    if (state.gap_first==state.fsize) return;
    if (state.fsize==MAX_SIZE){
        state.scr_first=MAX_SIZE-1;
        state.gap_first=MAX_SIZE-1;
        state.tail=MAX_SIZE;
        update_display();
        line_up();
        line_end();
    }else{
        saddr=state.tail;
        maxlen=min(CHAR_PER_LINE,state.tail - state.gap_first);
        while (saddr < MAX_SIZE){
            size=min(maxlen,MAX_SIZE-saddr);
            sram_read_block(saddr,line,size);
            if ((saddr+size)<MAX_SIZE){
                sram_clear_block(saddr,size);
                sram_write_block(state.gap_first,line,size);
                saddr+=size;
                state.gap_first+=size;
            }else{
                i=size-1;
                while (i && line[i]) i--;
                i++;
                sram_write_block(state.gap_first,line,i);
                state.gap_first+=i;
                sram_clear_block(saddr,i);
                saddr+=i;
                break;
            }
        }
        state.scr_first=state.gap_first;
        state.tail=saddr;
        state.scr_col=0;
        state.scr_line=0;
        update_display();
    }
}//f()

static void page_up(){
    int i=0;

    while (state.scr_line) line_up();
    i=LINE_PER_SCREEN;
    dis_video=true;
    while (state.scr_first && i){
        line_up();
        i--;
    }
    dis_video=false;
}

static void page_down(){
     while (state.gap_first<(state.fsize-1) && state.scr_line<LAST_LINE){
         line_down();
     }
    if ((state.scr_line==LAST_LINE) && (state.scr_col==0)){
        state.scr_first=state.gap_first;
        state.scr_line=0;
    }
    update_display();
}


static void word_right(){
    uint8_t c, scr_col;
    uint8_t line[CHAR_PER_LINE];
    
    if (state.scr_col==strlen((char*)&screen[state.scr_line])) return;
    scr_col=state.scr_col;
    c=screen[state.scr_line][scr_col++];
    if (!isalnum(c)){
        while (c && !isalnum(c)){
            c=screen[state.scr_line][scr_col++];;
        }
    }else{
        while (c  && isalnum(c)){
            c=screen[state.scr_line][scr_col++];
        }    
    }
    scr_col--;
    if (scr_col>state.scr_col){
        sram_read_block(state.tail,line,scr_col-state.scr_col);
        sram_clear_block(state.tail,scr_col-state.scr_col);
        state.tail+=scr_col-state.scr_col;
        sram_write_block(state.gap_first,line,scr_col-state.scr_col);
        state.gap_first+=scr_col-state.scr_col;
        state.scr_col=scr_col;
        set_cursor(scr_col,state.scr_line);
    }
}

static void word_left(){
    uint8_t c, scr_col, diff;
    uint8_t line[CHAR_PER_LINE];
    
    if (!state.scr_col) return;
    scr_col=state.scr_col-1;
    c=screen[state.scr_line][scr_col];
    if (!isalnum(c)){
        while (scr_col && !isalnum(c)){
           --scr_col;
           c=screen[state.scr_line][scr_col];
        }
    }else{
        while (scr_col && isalnum(c)){
           --scr_col;
           c=screen[state.scr_line][scr_col];
            
        }
    }
    if (scr_col) scr_col++;
    if (scr_col<state.scr_col){
        diff=state.scr_col-scr_col;
        state.gap_first-=diff;
        sram_read_block(state.gap_first,line,diff);
        sram_clear_block(state.gap_first,diff);
        state.tail-=diff;
        sram_write_block(state.tail,line,diff);
        state.scr_col=scr_col;
        set_cursor(scr_col,state.scr_line);
    }
}

static void jump_to_line(uint16_t line_no){
    uint16_t llen,count;
    uint8_t line[CHAR_PER_LINE];
    
    file_home();
    if (!line_no) return;
    line_no--;
    if (line_no<LINE_PER_SCREEN){
        while(state.scr_line<line_no) line_down();
        return;
    }
    _disable_video();
    count=0;
    while ((state.tail<MAX_SIZE) && (count<line_no)){
        llen=get_line_forward(line,state.tail);
        sram_write_block(state.gap_first,line,llen+1);
        state.gap_first+=llen+1;
        sram_clear_block(state.tail,llen+1);
        state.tail+=llen+1;
        count++;
    }
    state.scr_first=state.gap_first;
    set_cursor(0,0);
    update_display();
}//f

static void goto_line(){
    uint8_t llen;
    char line[16];
    uint16_t line_no=0;
    
    info_colors();
    set_cursor(0,0);
    clreol();
    print("goto line: ");
    llen=readln(line,16);
    editor_colors();
    if (llen){
        line_no=atoi((const char*)line);
        jump_to_line(line_no);
    }else{
        print_line(0);
    }
    
}
