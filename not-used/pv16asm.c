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
*   Nom:  pv16asm.c
*   Description: assembleur pour la console PV16SOG
*   auteur: Jacques Deschenes
*   Date:  2015-12-13
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <setjmp.h>
#include "Hardware/hardware.h"
#include "filesys.h"
#include "text.h"
#include "Hardware/spi_ram.h"
#include "errors.h"
#include "Hardware/ps2_kbd.h"
#include "Hardware/systicks.h"

static jmp_buf failed;

static char upper(char c){
    if (c>='a' && c<='z')
        return c-32;
    else
        return c;
}//f()

typedef struct data_node{
	char *name;
	union {
        uint16_t cast;
		uint16_t addr;
		uint16_t pc;
		int16_t value;
		char *defn;
	};
	struct data_node *next;
}node_t;

typedef enum token {eNONE,eSYMBOL,eLABEL,eNUMBER,eSTRING,eADDOP,eMULOP,eMNEMONIC,
                    eDIRECTIVE,eCOMMA,eLPAREN,eRPAREN,eLBRACKET,
                    eRBRACKET,eDOT,eCHAR,eADDR,eVAR} token_t;

static token_t tok_id;


static node_t *label_list;   // liste des étiquettes
static node_t *forward_list; // référence avant
static node_t *symbol_list; // liste des EQU
static node_t *define_list; // liste des DEF

#define add_label(name,addr)   add_node(name,addr,label_list)
#define add_forward_ref(name,pc) add_node(name,pc,forward_list)
#define add_symbol(name,value)  add_node(name,(uint16_t)value,symbol_list)
#define add_define(name,str) add_node(name,(uint16_t)str,define_list)
#define search_label(name)   search_list(name,label_list)
#define search_ref(name)  search_list(name,forward_list)
#define search_symbol(name) search_list(name,symbol_list)
#define search_define(name) search_list(name,define_list)


static int pc; // compteur ordinal
static int line_no; //no de ligne en cours d'analyse
static int tok_count; //ordre du jeton sur la ligne
static char *line;
static int code_size=0; 
//static bool error_stop=false;

#define PROG_SPACE (8192)   
#define LINE_LEN_MAX 128

typedef union{
    char symbol[42];
    uint16_t addr;
    int16_t number;
} tok_value_t;

static unsigned char *binary;
static char *tok_value;

#define KW_COUNT (47)

static const char *mnemonics[KW_COUNT]={
    "NOP","CLS","RET","SCR","SCL","EXIT","SAVE","RSTR","SCD","JP","CALL",
    "SHR","SHL","SKP","SKNP","SE","SNE","ADD","SUB","SUBN","OR","AND","XOR",
    "RND","TONE","PRT","PIXI","LD","DRW","NOISE","PUSH","POP","SCRX","SCRY",
    "SCU","BSET","BCLR","BINV","BTSS","BTSC","GPIX","PRN","PRC","SCUR","KEY",
    "SET","SETP"};

typedef enum Mnemo {eNOP,eCLS,eRET,eSCR,eSCL,eEXIT,eSAVE,eRSTR,eSCD,eJP,eCALL,
                    eSHR,eSHL,eSKP,eSKNP,eSE,eSNE,eADD,eSUB,eSUBN,eOR,eAND,
                    eXOR,eRND,eTONE,ePRT,ePIXI,eLD,eDRW,eNOISE,ePUSH,ePOP,
                    eSCRX,eSCRY,eSCU,eBSET,eBCLR,eBINV,eBTSS,eBTSC,eGPIX,
                    ePRN,ePRC,eSCUR,eKEY,eSET,eSETP} mnemo_t;
						 
#define DIR_COUNT (7)						 
static const char *directives[]={"DB","DW","ASCII","EQU","DEF","END","SPACE"};

typedef enum dir_id {eDB,eDW,eASCII,eEQU,eDEF,eEND,eSPACE} directive_t;

// search word in a list 
static int search_word(char *target, const char *list[], int list_count){
	int i=0;
	while (i<list_count){
		if (!strcmp(target,list[i])) break;
		i++;
	}
	return i;
	
}

static bool letter(char c){
	return ((c>='A') && (c<='Z')) || ((c>='a') && (c<='z'));
}

bool digit(char c){
	return (c>='0') && (c<='9');
}

static bool alnum(char c){
	return letter(c) || digit(c);
}

static bool hex(char c){
	return digit(c) || ((c>='A')&&(c<='F'));
}

//static bool identifier(char *name){
//	if (!letter(*name)) return false;
//	name++;
//	while (*name){
//		if (!(alnum(*name) || *name=='_')) return false;
//		name++;
//	}
//	return true;
//}

//caractères qui séparent les unitées lexicale
static bool separator(char c){
	return strchr("()[]+-*/%,",c);
}

static void next_token();
static int16_t expression();


typedef enum ASM_ERROR_CODE {eSYNTAX, eALIGN, eMEMORY,eUNKNOWN,eEXPRESSION,eBADARG,
    eRANGE,eBADMNEMO,eNOTREF,eDUPLICATE,eBINFILE,eERRNONE} error_code_t;

__eds__ static PSTR error_msg[eERRNONE]={
"syntax error",
"alignement error",
"memory overflow",
"unknown symbol",
"bad expression",
"bad argument",
"out of range",
"bad mnemonic",
"undefined reference",
"duplicate label",
"binaray file creation error",
};

//gestion des exceptions
static void throw(error_code_t error_code){
	char msg[40];
    
    new_line();
    pstrcpy(msg,error_msg[error_code]);
    print(msg);
    print("\nat line: ");
    print_int(line_no+1,0);
    print("\ntok_id: ");
    print_int(tok_id,0);
    print("\ntok_value: ");
    if (tok_id==eNUMBER){
        print_int(*tok_value,0);
    }else{
        print(tok_value);
    }
    print("\ntoken number: ");
    print_int(tok_count,0);
    new_line();
    longjmp(failed, error_code);
}


//convertie une chaine hexadécimale
//en entier positif
static int16_t htoi(char *hnbr){
	unsigned int n=0;
	while (hex(*hnbr)){
		n*=16;
		n+=*hnbr<='9'?*hnbr-'0':*hnbr-'A'+10;
		hnbr++;
	}
	return n;
}

// convertie une chaine binaire
// en entier positif
static int16_t btoi(char *bnbr){
	unsigned int n=0;
	while ((*bnbr=='1')||(*bnbr=='.')||(*bnbr=='0')){
		n <<=1;
		n += *bnbr=='1';
		bnbr++;
	}
	return n;
}

//convertie un token numérique
// en entier
static int16_t token_to_i(){
	switch(tok_value[0]){
	case '$': //hexadécimal
		return htoi(tok_value+1);
	case '#': // binaire
		return btoi(tok_value+1);
	default:
		return atoi(tok_value);
	}
}

static node_t *add_node(char *name, uint16_t value,node_t *list){
	node_t *n;
	
    n=dalloc(sizeof(node_t)); 
    if (!n){
        throw(eMEMORY);
       // return 0;
    }
	n->next=list;
	n->cast=value;
	n->name=name;
	return n;
}

static node_t *search_list(char *name, node_t *list){
	node_t *node;
	node=list;
	while (node){
		if (!strcmp(name,node->name)) break;
		node=node->next;
	}
	return node;
}


static bool space(char c){
   return ((c==' ')||(c=='\t'));
}

#define SBUFFER_SIZE 64
typedef struct{
    uint16_t fsize; // grandeur du fichier
    uint16_t saddr; // position dans la SPIRAM
    uint8_t sindex; // position dans sbuffer
    uint8_t count;  // nombre d'octets dans sbuffer
    bool eof;  // indicateur de fin de fichier.
    char sbuffer[SBUFFER_SIZE];
} reader_t;
reader_t *reader;

//rempli le buffer
static bool read_sram(){
    if (reader->saddr==reader->fsize){
        reader->eof=true;
        return false;
    }
    reader->count=min(SBUFFER_SIZE,reader->fsize-reader->saddr);
    sram_read_block(reader->saddr,(uint8_t*)reader->sbuffer,reader->count);
    reader->sindex=0;
    reader->saddr+=reader->count;
    return true;
}

static char reader_getc(){
    char c;
    if (reader->eof) return -1;
    if (reader->sindex < reader->count){
        c=reader->sbuffer[reader->sindex++];
    }else if (read_sram()){
        c=reader->sbuffer[reader->sindex++];
    }else{
        c=-1;
    }
    return c;
}

inline static void reader_ungetc(){
    reader->sindex--;
}

static void skip_space(){
    char c;
    c=reader_getc();
    while (space(c)){
        //put_char(c);
        c=reader_getc();
    }
    if (!reader->eof) reader_ungetc();
}

// place le pointeur au début de la ligne suivante.
static void skip_comment(){
    char c=0;
    while (((c=reader_getc())>'\n') || (c=='\t'));//put_char(c);
    if ((tok_id!=eNONE) && (c=='\n')) reader_ungetc();
}


static unsigned parse_quote(){
    char c;
    //put_char('\'');
    c=reader_getc();
    //put_char(c);
    if (c=='\\'){ 
        c=reader_getc();
        //put_char(c);
        switch (c){
            case '\\':
                tok_value[0]='\\';
                break;
            case '\'':
                tok_value[0]='\'';
            case 'n':
            case 'r':
                tok_value[0]='\n';
                break;
            case 't':
                tok_value[0]='\t';
                break;
            default:
                throw(eSYNTAX);
        }
    }else{
        tok_value[0]=c;
    }
    if ((c=reader_getc())!='\''){
        throw(eSYNTAX);
    }else{
        //put_char('\'');
    }
    return 1;
}

static unsigned parse_string(){
	bool quote=false;
	bool escape=false;
	unsigned i=0;
	char c;
    
    //put_char('"');
    c=reader_getc();
	while (!reader->eof && !quote && (c>0)){
        //put_char(c);
        if (!escape){
            switch (c){
            case '\\':
                escape=true;
                break;
            case '"':
                quote=true;
                break;
            default:
                tok_value[i++]=c;
            }
        }else{
            switch (c){
                case '"':
                    tok_value[i++]=c;
                    break;
                case 't':
                    tok_value[i++]='\t';
                    break;
                case 'n':
                case 'r':
                    tok_value[i++]='\n';
                    break;
                default:
                    throw(eSYNTAX);
                    //return i;
            }
            escape=false;
        }
		c=reader_getc();
	}//while
    tok_value[i++]=0;
	if (quote){
        reader_ungetc();
    } else{
        throw(eSYNTAX);
    }
	return i;
}

static bool unget_token;
static void next_token(){
	unsigned state=0;
	unsigned i=0;
    char c;
    
    if (unget_token){
        unget_token=false;
        return;
    }
	tok_id=eNONE;
    tok_value[0]=0;
	skip_space();
    while (state<5) {
        c=upper(reader_getc());
        switch (c){
            case -1:
                state=5;
                break;
            case ' ':
            case '\t':
                reader_ungetc();
                state=5;
                break;
            case '\r':
            case '\n':
                if (tok_id!=eNONE){
                    reader_ungetc();
                }
                state=5;
                break;
            case ';':
                if (tok_id==eNONE){
                    //put_char(';');
                    skip_comment();
                }else{
                    reader_ungetc();
                }
                state=5;
                break;
            default:;
        }//switch
        if (state==5){
            if (tok_id==eNONE){
                //new_line();
                line_no++;
            }
            break;
        }
		switch(state){
		case 0:
			switch(c){
			case '+':
			case '-':
				tok_id=eADDOP;
				tok_value[i++]=c;
				state=5;	
				break;
			case '*':
			case '/':
			case '%':
				tok_id=eMULOP;
				tok_value[i++]=c;
				state=5;	
				break;
			case '.':
				tok_id=eDOT;
				tok_value[i++]=c;
				state=5;
				break;
			case '[':
				tok_id=eLBRACKET;
				tok_value[i++]=c;
				state=5;	
				break;
			case ']':
				tok_id=eRBRACKET;
				tok_value[i++]=c;
				state=5;	
				break;
			case '(':
				tok_id=eLPAREN;
				tok_value[i++]=c;
				state=5;	
				break;
			case ')':
				tok_id=eRPAREN;
				tok_value[i++]=c;
				state=5;	
				break;
			case ',':
				tok_id=eCOMMA;
				tok_value[i++]=c;
				state=5;	
				break;
			case '"':
				i=parse_string();
				tok_id=eSTRING;
                c=0;
				state=5;
				break;
            case '\'':
                i=parse_quote();
                tok_id=eCHAR;
                c=0;
                state=5;
                break;
			case '$':
				tok_id=eNUMBER;
				tok_value[i++]=c;
				state=1; // nombre hexadécimal
				break;
			case '#':
				tok_id=eNUMBER;
				tok_value[i++]=c;
				state=2; // nombre binaire
			default:
				if (letter(c)||(c=='_')){
					tok_id=eSYMBOL;
					tok_value[i++]=c;
					state=4; // symbole alphanumérique
				}else if (digit(c)){
					tok_id=eNUMBER;
					tok_value[i++]=c;
					state=3; // nombre décimal
				}
			}//switch
			break;
		case 1: // nombre hexadécimal
			if (hex(c)){
				tok_value[i++]=c;
			}else if (separator(c)){
                reader_ungetc();
                c=0;
				state=5;
			}else{
				throw(eSYNTAX);
                //state=5;
			}
			break;
		case 2: // nombre binaire
			if ((c=='1') || (c=='0') || (c=='.')){
				tok_value[i++]=c;
			}else if (separator(c)){
                reader_ungetc();
                c=0;
				state=5;
			}else{
				throw(eSYNTAX);
                //state=5;
			}
			break;
		case 3: // nombre décimal
			if (digit(c)){
				tok_value[i++]=c;
			}else if (separator(c)){
                reader_ungetc();
                c=0;
				state=5;
			}else{
				throw(eSYNTAX);
                //state=5;
			}
			break;
		case 4: // symbole alphanumérique
			if (alnum(c) || (c=='_')){
				tok_value[i++]=c;
			}else if (c==':'){
				tok_id=eLABEL;
				state=5;
			}else if (separator(c)){
                tok_id=eSYMBOL;
                reader_ungetc();
                c=0;
				state=5;
			}else{
				throw(eSYNTAX);
			}
			break;
		}//switch
        //if (c)put_char(c);
    } //while
 	tok_value[i]=0;
    if (tok_id==eNUMBER){
       *(int16_t*)tok_value=token_to_i();
    }
    tok_count++;
}//f())

// ce n'est pas un EQU ni un DEF donc
// remplace la chaîne par l'adresse du label
// si le label n'existe pas créer une référence avant
// et met zéro dans tok_value.
static void process_label(){
    node_t *n;
    char *name;

    n=search_list(tok_value,label_list);
    if (n){
        *((uint16_t*)tok_value)=n->addr;
        tok_id=eADDR;
    }else{ // étiquette inexitante on l'ajoute comme référence avant.
        name=dalloc(strlen(tok_value)+1);
        if (!name){
            throw(eMEMORY);
        }
        strcpy(name,tok_value);
        forward_list=add_forward_ref(name,pc);
        *((uint16_t*)tok_value)=0;
        tok_id=eADDR;
        
    }
}

//sie le symbol est un  EQU
// substitution par la valeur entière
static bool try_equ(){
    node_t *n;
    
    n=search_list(tok_value,symbol_list);
    if (n){
        *(int16_t*)tok_value=n->value;
        tok_id=eNUMBER;
        return true;
    }
    return false;
}


//si le symobole est un DEF
//substition par la chaîne n->defn
static bool try_def(){
    node_t *n;
    
    n=search_list(tok_value,define_list);
    if (n){
        strcpy(tok_value,n->defn);
        return true;
    }
    return false;
}

#define var_index() (tok_value[1]>'9'?10+tok_value[1]-'A':tok_value[1]-'0')
//vérifie si le symbole est de forme VX
static bool try_var(){
    if ((tok_value[2]==0) && tok_value[0]=='V' && hex(tok_value[1])){
        tok_id=eVAR;
        tok_value[0]=var_index();
        return true;
    }else{
        return false;
    }
}

// le jeton doit-être un eSYMBOL
static void expect_symbol(){
    next_token();
    if (tok_id!=eSYMBOL) throw(eBADARG);
}


static void expect_var(){
    expect_symbol();
    if (!(try_var() || (try_def() && try_var()))){
        throw(eBADARG);
    }
}

// le prochain jeton devrait-être eCOMMA.
static void expect_comma(){
    next_token();
    if (tok_id!=eCOMMA){
        throw(eSYNTAX);
    }
}

// le prochain jeton doit-être eNUMBER
static void expect_number(){
    next_token();
    if (tok_id!=eNUMBER){
        throw(eBADARG);
    }
}

// le prochain jeton doit-être eADDR
static void expect_addr(){
    next_token();
    if (tok_id!=eSYMBOL){
        throw(eBADARG);
    }
    process_label();
}


// le prochain jeton doit-être eRBRACKET
static void expect_rbracket(){
    next_token();
    if (tok_id!=eRBRACKET){
        throw(eSYNTAX);
    }
}

// le dernier tok_id était eLBRACKET
// les 2 suivants doivent être 'I' et ']'
static void expect_indirect_I(){
    expect_symbol();
    if ((tok_value[0]=='I') && (tok_value[1]==0)){
        expect_rbracket();
    }else
        throw(eSYNTAX);
}

//devrait-être à la fin de ligne
static void expect_none(){
    next_token();
    if (tok_id!=eNONE){
        throw(eSYNTAX);
    }
}

static void data_byte(){
	int8_t n;
	
	do{
	   n=expression();
	   binary[pc++]=n&0xff;
	   code_size++;
       next_token();
       if (tok_id==eNONE) break;
	   if (tok_id!=eCOMMA){
           throw(eSYNTAX);
           //break;
       }
   }while((pc<PROG_SPACE-1) && !(tok_id==eNONE));
   if (pc&1) {binary[pc++]=0;code_size++;} //aligne le compteur ordinateur sur adresse paire.
}

static void data_word(){
	int16_t n;
	
	do{
		n = expression();
		binary[pc++]=(n>>8)&0xff;
		binary[pc++]=n&0xff;
		code_size+=2;
        next_token();
        if (tok_id==eNONE) break;
		if (tok_id!=eCOMMA){
            throw(eSYNTAX);
            //break;
        }
	}while((pc<PROG_SPACE-1) && !(tok_id==eNONE));
}

static void data_ascii(){
	unsigned i=0;
	
	next_token();
	if (tok_id!=eSTRING){
        throw(eSYNTAX);
        //return;
    }
	while ((pc<(PROG_SPACE-1)) && tok_value[i]){
		binary[pc++]=tok_value[i++];
		code_size++;
    }
    binary[pc++]=0;code_size++;
	if (pc&1){binary[pc++]=0;code_size++;} //aligne le compteur ordinateur sur adresse paire.
    next_token();
}

static void reserve_space(){
    int16_t i;
    
    i=expression();
    if (i>0){
        pc+=i;
        if (pc&1)pc++;
    }else{
        throw(eBADARG);
    }
}//f()

static void equate(){
	node_t *symbol;
	char *name;
    
	expect_symbol();
    symbol=search_symbol(tok_value);
    if (!symbol){
        name=dalloc(strlen(tok_value)+1);
        if (!name){
            throw(eMEMORY);
            //return;
        }
        strcpy(name,tok_value);
        if ((symbol = add_symbol(name,0))){
            symbol->value=expression();
            symbol_list=symbol;
        }
    }else{
        symbol->value=expression();
    }
    expect_none();
	
}

static void define(){
	char *def_str,*name;
    
	expect_symbol();
    name=dalloc(strlen(tok_value)+1);
    if (!name) {
        throw(eMEMORY);
    }
    strcpy(name,tok_value);
    next_token();
    if ((tok_id==eSYMBOL) || (tok_id==eSTRING)){
        if ((def_str=dalloc(strlen(tok_value)+1))){
            strcpy(def_str,tok_value);
            define_list=add_define(name,def_str);
            next_token();
        }
    }else{
        throw(eSYNTAX);
    }
}

static int16_t factor(){
	int16_t n=0;
	char c;
	
	next_token();
	switch(tok_id){
	case eADDOP:
		c=tok_value[0];
		n=factor();
		if (c=='-') n=-n;
		break;
	case eSYMBOL:
		if (try_equ())
		   n=*(int16_t*)tok_value; 
		else 
            throw(eEXPRESSION);
		break;
	case eNUMBER:
		n=*(int16_t*)tok_value;
		break;
    case eCHAR:
        n=tok_value[0];
        break;
	case eLPAREN:
		n=expression();
		next_token();
		if (tok_id!=eRPAREN) throw(eEXPRESSION);
		break;
	default:
		throw(eEXPRESSION);
	}
	return n;
}

static int16_t term(){
    int16_t n;
    
	n=factor();
	next_token();
	while (tok_id==eMULOP){
		switch (tok_value[0]){
		case '*': 
			n*=factor();
			break;
		case '/':
			n/=factor();
			break;
		case '%':
			n%=factor();
			break;
		}//switch
		next_token();
	}//while
    unget_token=true;
	return n;
}

static int16_t expression(){
	int16_t n;

	n=term();
	next_token();
	while (tok_id==eADDOP){
		switch(tok_value[0]){
		case '+':
			n += term();
			break;
		case '-':
			n -= term();
			break;
		}//switch
		next_token();
	}//while
    unget_token=true;
	return n;
}

//forme opcode: FX55
// LD [I], VX
// sauvegarde les registres V0-VX dans l'espace programme pointée par [I]
static void store_vars(){
    
    expect_indirect_I();
    expect_comma();
    expect_var();
    binary[pc++]=0xF0|tok_value[0];
    binary[pc++]=0x55;
    expect_none();
}

// LD DT, VX  | FX15
// LD ST, VX  | FX18
static void load_timer(){
    uint8_t b2=0;
    
    if (tok_value[0]=='D'){ // LD DT, VX | FX15
        b2=0x15;
    }else if (tok_value[0]=='S'){//LD ST, VX | FX18
        b2=0x18;
    }else{
        throw(eBADARG);
    }
    expect_comma();
    expect_var();
    binary[pc++]=0xF0+tok_value[0];
    binary[pc++]=b2;
    expect_none();
}

// LD VX, KK    | 6XKK
// LD VX, 'c'   | 6XKK
// LD VX, VY    | 8XY0
// LD VX, K     | FX0A
// LD VX, [I]   | FX65
static void load_vx(){
    int vx;
    
    vx=tok_value[0];
    expect_comma();
    next_token();
    switch(tok_id){
        case eADDOP:
        case eNUMBER:
            // LD VX, KK | 6XKK
            unget_token=true;
            binary[pc++]=0x60+vx;
            binary[pc++]=expression(); //print_int(binary[pc-1]);
            expect_none();
            break;
        case eCHAR: // LD VX, 'c'
            binary[pc++]=0x60+vx;
            binary[pc++]=tok_value[0];
            expect_none();
            break;
        case eLBRACKET: // LD VX, [I] | FX65
            expect_indirect_I();
            binary[pc++]=0xF0+vx;
            binary[pc++]=0x65;
            expect_none();
            break;
        case eSYMBOL:
            if (try_equ()){ 
                //LD VX, KK
                unget_token=true;
                binary[pc++]=0x60+vx;
                binary[pc++]=expression();
                expect_none();
            }else if (try_var()|| (try_def() && try_var())){
                binary[pc++]=0x80+vx;
                binary[pc++]=tok_value[0]<<4;
                expect_none();
            }else  if ((tok_value[1]==0) && (tok_value[0]=='K')){
             // LD VX, K |  FX0A 
                binary[pc++]=0xF0+vx;
                binary[pc++]=0xA;
                expect_none();
            }else if (tok_value[0]=='D' && tok_value[1]=='T' && tok_value[2]==0){
             // LD VX, DT | FX07
                binary[pc++]=0xF0|vx;
                binary[pc++]=0x7;
                expect_none();
            }else{
                throw(eBADARG);
            }
            break;
        default:
            throw(eBADARG);
    }//switch
    
}

// LD I, label | ANNN
static void load_I(){
    uint16_t addr;
    
    expect_comma();
    expect_addr();
    addr=*((uint16_t*)tok_value);
    binary[pc++]=0xA0|((addr>>9)&0xf);
    binary[pc++]=(addr>>1)&0xff;
    expect_none();
}

// LD VX, KK    | 6XKK
// LD VX, VY    | 8XY0
// LD VX, DT    | FX07
// LD VX, [I]   | FX65
// LD I, label  | ANNN
// LD [I], VX   | FX55
// LD DT, VX    | FX15
// LD ST, VX    | FX18
// LD VX, K     | FX0A
static void code_load(){
    next_token();
    switch(tok_id){
        case eLBRACKET:
            store_vars();
            break;
        case eSYMBOL:
            if (try_var() || (try_def() && try_var())){
                load_vx();
            }else if ((tok_value[1]=='T') && (tok_value[2]==0)){
                load_timer();
            }else if ((tok_value[0]=='I') && tok_value[1]==0){
                load_I();
            }else{
                throw(eSYNTAX);
            }
            break;
        default:
            throw(eSYNTAX);
            
    }
}

// forme opcode: 8XY4 | 7XKK | FX1E
// ADD I, VX    | FX1E
// ADD VX, VY   | 8XY4
// ADD VX, KK   | 7XKK
static void code_add(){
    expect_symbol();
    if (try_var()|| (try_def() && try_var())){
        binary[pc++]=0x70+tok_value[0];
        expect_comma();
        next_token();
        if (try_var() || (try_def() && try_var())){
            binary[pc-1]+=0x10;
            binary[pc++]=(tok_value[0]<<4)+4;
            expect_none();
        }else{
            unget_token=true;
            binary[pc++]= expression()&0xff;
            expect_none();
        }
    }else if (tok_value[0]=='I' && tok_value[1]==0){
        expect_comma();
        expect_var();
        binary[pc++]=0xf0+tok_value[0];
        binary[pc++]=0x1e;
        expect_none();
    }else{
        throw(eBADARG);
    }
}

// forme opcode:  9XY1  | 9XY5
// TONE VX, VY          | 9XY1
// TONE VX, VY, WAIT    | 9XY5
static void code_tone(){
    expect_var();
    binary[pc++]=0x90+tok_value[0];
    expect_comma();
    expect_var();
    binary[pc]=tok_value[0]<<4;
    next_token();
    if (tok_id==eNONE){
        binary[pc++]|=1;
    }else  if (tok_id==eCOMMA){
        expect_symbol();
        if (!strcmp("WAIT",tok_value)){
            binary[pc++]|=5;
            expect_none();
        }else{
            throw(eBADARG);
        }
    }else{
        throw(eSYNTAX);
    }
}

//forme opcode: DXYN
// DRW  VX,VY,N | DXYN
static void code_draw(){
    uint8_t vy;
    
    expect_var();
    binary[pc++]=0xD0+tok_value[0];
    expect_comma();
    expect_var();
    vy=tok_value[0];
    expect_comma();
    next_token();
    if (tok_id==eNUMBER){
        binary[pc++]=(vy<<4)+((*(int16_t*)tok_value)&0xf);
    }else if (tok_id==eSYMBOL && try_equ()){
        binary[pc++]=(vy<<4)+((*(int16_t*)tok_value)&0xf);
    }else{
        throw(eBADARG);
    }
}

//forme opcode:  CXYC | CXKK
// SE VX, KK    | 3XKK
// SE VX, VY    | 5XY0
// SNE VX, KK   | 4XKK
// SNE VX, VY   | 9XY0
static void code_skip_vx(uint8_t b1){
    expect_var();
    binary[pc++]=b1+tok_value[0];
    expect_comma();
    next_token();
    if ((tok_id==eSYMBOL) && (try_var() || (try_def() && try_var()))){
        if (b1==0x30){
            binary[pc-1]+=0x20;
        }else{
            binary[pc-1]+=0x50;
        }
        binary[pc++]=tok_value[0]<<4;
        expect_none();
    }else{
        unget_token=true;
        binary[pc++]=expression()&0xff;
        expect_none();
    }
} 

// forme opcode: CX0C
// SHR VX       | 8X06
// SHL VX       | 8X0E
// NOISE VX     | 9X04
// PUSH VX      | 9X06
// POP VX       | 9X07
// SCRX VX      | 9X08
// SCRY VX      | 9X09
// SHL VX, P    | 9XPF
static void code_op1_vx(uint8_t b1, uint8_t b2){
    expect_var();
    binary[pc++]=b1|tok_value[0];
    binary[pc++]=b2;
    next_token();
    if (tok_id==eNONE) return;
    if ((tok_id==eCOMMA) && (b2==0xe)){ // SHL VX, P
        expect_number();
        binary[pc-2]-=0x10;
        binary[pc-1]=(((*(int16_t*)tok_value)&0xf)<<4)|0xf;
        expect_none();
    }
}

// PUSH I       | F075
// POP I        | F085
static void code_I_stack_op(uint8_t b2){
    if ((tok_value[0]=='I')&&(tok_value[1]==0)){
        binary[pc++]=0xf0;
        binary[pc++]=b2;
        expect_none();
    }else{
        unget_token=true;
        code_op1_vx(0xf0,b2);
    }
}


// forme opcode: 00CZ
// PRN VZ   | 002Z
// PRC VZ   | 003Z
// KEY VZ   | 004Z
// SCU VZ   | 005Z
// SCD VZ   | 006Z
static void code_op1_vz(uint8_t b2){
    expect_var();
    binary[pc++]=0;
    binary[pc++]=b2+tok_value[0];
    expect_none();
}

//forme opcode: CXYC
// OR VX, VY    | 8XY1
// AND VX, VY   | 8XY2
// XOR VX, VY   | 8XY3
// SUB VX, VY   | 8XY4
// SUBN VX, VY  | 8XY7
// SCUR VX, VY  | 8XYD
// PIXI VX, VY  | 9XY3
// GPIX VX, VY  | 9XYF
static void code_op2_vxvy(uint8_t b1, uint8_t b2){
    expect_var();
    binary[pc++]=b1|tok_value[0];
    expect_comma();
    expect_var();
    binary[pc++]=b2|(tok_value[0]<<4);
    expect_none();
}

// forme opcode: CXPC
// BSET VX, P   | 9XPA
// BCLR VX, P   | 9XPB
// BINV VX, P   | 9XPC
// BTSS VX, P   | 9XPD
// BTSC VX, P   | 9XPE
static void code_op2_vxp(uint8_t b1, uint8_t b2){
    expect_var();
    binary[pc++]=b1|tok_value[0];
    expect_comma();
    expect_number();
    binary[pc++]=b2|(((*(int16_t*)tok_value)&0xf)<<4);
    expect_none();
}

// forme opcode: 00CN
// SET BG, N  | 008N
// SET FG, N  | 007N
// SET BG, VZ | 009Z
// SET FG, VZ | 00AZ
static void code_set(){
    uint8_t b2=0;
    expect_symbol();
    if ((tok_value[2]==0) && (tok_value[1]=='G')){
        if (tok_value[0]=='F'){
            b2=0x70;
        }else if (tok_value[0]=='B'){
            b2=0x80;
        }else{
            throw(eBADARG);
        }
    }
    binary[pc++]=0;
    expect_comma();
    next_token();
    if ((try_def() && try_var())||try_var()){
        b2+=0x20;
        binary[pc++]=b2|var_index();
        expect_none();
    }else{
        unget_token=true;
        binary[pc++]=b2|(expression()&0xf);
        expect_none();
    }
}

// forme opcode: BNNN
// JP V0, label
static void indexed_jump(){
    uint16_t addr;
    
    if ((tok_value[0]==0)){
        expect_comma();
        expect_addr();
        addr=*((uint16_t*)tok_value);
        binary[pc++]=0xB0|((addr>>9)&0xf);
        binary[pc++]=(addr>>1)&0xff;
        expect_none();
    }else{
        throw(eSYNTAX);
     }
}

// forme opcode: 1NNN
// JP .[(-|+)n]
static void relative_jump(){
    int16_t n;
    
    next_token();
    switch(tok_id){
        case eNONE:
            binary[pc]=0x10|((pc>>9)&0xff);
            binary[pc+1]=(pc>>1)&0xff;
            pc+=2;
            break;
        case eADDOP:
            unget_token=true;
            n=expression();
            n=pc+(n*2);
            binary[pc++]=0x10|((n>>9)&0xff);
            binary[pc++]=(n>>1)&0xff;
            expect_none();
            break;
        default:
            throw(eSYNTAX);
    }
}

// forme opcode: CNNN
// JP label   |1NNN
// JP V0, label  | BNNN
// JP .     | 1NNN
// JP .(+|-)n   | 1NNN
static void code_jump(){
    uint16_t addr;
    
    next_token();
    if (tok_id==eDOT){
        relative_jump();
    }else if (tok_id==eSYMBOL){
        if (try_var() || (try_def() && try_var())){
            indexed_jump();
        }else {
            process_label();
            if (eADDR){
                addr=*((uint16_t*)tok_value);
                binary[pc++]=0x10+(addr>>9);
                binary[pc++]=(addr>>1)&0xff;
                expect_none();
            }
        }
    }else{
        throw(eBADARG);
    }
}

// forme opcode:  2NNN
// CALL label
static void code_call(){
    uint16_t addr;
    
    expect_addr();
    addr=*((uint16_t*)tok_value);
    binary[pc++]=0x20+(addr>>9);
    binary[pc++]=(addr>>1)&0xff;
    expect_none();
}

// forme opcode: 00CC
// NOP  | 0000
// PRT  | 0001
// CLS  | 0015
// RET  | 00EE
// SCR  | 00FB
// SCL  | 00FC
// EXIT | 00FD
// SAVE | 00FE
// RSTR | 00FF
static void code_op0(uint8_t b2){
    binary[pc++]=0;
    binary[pc++]=b2;
    expect_none();
}

static void process_mnemonic(int n){
    switch(n){
        // opérations sans arguments
        case eNOP: // NOP ne fait rien
            code_op0(0);
            break;
        case ePRT:  // PRT imprime texte pointé par I à la position coourante du curseur
            code_op0(1);
            break;
        case eCLS: // CLS efface écran
            code_op0(0xe0);
            break;
        case eRET: // RET sortie de sous-routine
            code_op0(0xee);
            break;
        case eSCR: // SCR   glisse l'affiache vers la droite de 4 pixels.
            code_op0(0xfb);
            next_token();
            break;
        case eSCL: // SCL  glisse l'affichage vers la gauche de 4 pixels
            code_op0(0xfc);
            break;
        case eEXIT: // EXIT  fin d'exécution, sortie de pv16vm())
            code_op0(0xfd);
            break;
        case eSAVE: // SAVE  sauvegarde l'affichage dans la SRAM
            code_op0(0xfe);
            break;
        case eRSTR: // RSTR  restaure l'affichage à partir de la SRAM
            code_op0(0xff);
            break;
       // ***** opérations avec 1 argument *****
        case ePRN:  // PRN VZ   affiche l'entier dans VZ à la position du curseur
            code_op1_vz(0x20);
            break;
        case ePRC: // PRC VZ  affiche le caractère dans VZ à la position du curseur
            code_op1_vz(0x30);
            break;
        case eKEY: // KEY VZ  lecture d'une touche clavier dans VZ
            code_op1_vz(0x40);
            break;
        case eSCD: // SCD VZ   glisse l'affichage vers le bas de VZ pixels
            code_op1_vz(0x50);
            break;
        case eSCU: // SCU VZ   glisse l'affichage vers le haut de VZ pixels
            code_op1_vz(0x60);
            break;
        case eSET: // SET BG|FG, N  fixe couleur font et pixel.
            code_set();
            break;
        case eJP: // JP NNN| JP V0, NNN  saut vers l'adresse 2*NNN|2*(NNN+V0)
            code_jump();
            break;
        case eCALL: // CALL NNN appel de sous-routine à l'adresse 2*NNN
            code_call();
            break;
        case eSHR: // SHR VX  décale le registre VX vers la droite
            code_op1_vx(0x80,6);
            break;
        case eSHL: // SHL VX  décale le registre VX vers la gauche
            code_op1_vx(0x80,0xe); // peut aussi être SHL VX, P
            break;
        case eNOISE: // NOISE NN  bruit blanc d'une durée de 8*NN msec
            code_op1_vx(0x90,4);
            break;
        case ePUSH: // PUSH VX empile la valeur de VX
            next_token();
            if (tok_id==eSYMBOL){
                code_I_stack_op(0x75);
            }else{
                unget_token=true;
                code_op1_vx(0x90,6);
            }                             
            break;
        case ePOP: // POP VX  dépile dans VX
            next_token();
            if (tok_id==eSYMBOL){
                code_I_stack_op(0x85);
            }else{
                unget_token=true;
                code_op1_vx(0x90,7);
            }
            break;
        case eSCRX: // eSCRX VX | VX=HRES
            code_op1_vx(0x90,8);
            break;
        case eSCRY: // eSCRY VX | VX=VRES
            code_op1_vx(0x90,9);
            break;
        case eSKP:  // SKP VX  saut conditionnel à bouton VX enfoncé
            code_op1_vx(0xE0,0x9E);
            break;
        case eSKNP: // SKNP VX saut conditionnel à bouton VX relâché
            code_op1_vx(0xE0,0xA1);
            break;
        // ***** opérations à 2 arguments ****
        case eSE: // SE VX, VY|KK  saut conditionnel si arguments ==
            code_skip_vx(0x30);
            break;
        case eSNE: // SNE VX, VY|KK // saut conditionnel si arguments !=
            code_skip_vx(0x40);
            break;
        case eLD: // LD VX|I|[I]|ST|DT, VX|KK|[I]|DT|K|NNN
            code_load();
            break;
        case eADD: // ADD VX|I, VX|VY|KK   VX=VX+VY| I=I+VX
            code_add();
            break;
        case eOR: // OR VX, VY   VX=VX|VY
            code_op2_vxvy(0x80,1);
            break;
        case eAND: // AND VX, VY  VX=VX&VY
            code_op2_vxvy(0x80,2);
            break;
        case eXOR: // XOR VX, VY   VX=VX^VY
            code_op2_vxvy(0x80,3);
            break;
        case eSUB: // SUB VX, VY   VX=VX-VY
            code_op2_vxvy(0x80,5);
            break;
        case eSUBN: // SUBN VX, VY  VX= VY-VX
            code_op2_vxvy(0x80,7);
            break;
        case eSCUR: // SCUR VX, VY  positionne le curseur texte à VX=colonne, VY= ligne
            code_op2_vxvy(0x80,0xD);
            break;
        case eTONE: // TONE VX, VY [, WAIT] fait entendre une tonalité VX=freq. VY=durée
            code_tone();
            break;
        case eSETP: // SETP VX, VY  set pixel à la couleur FG
            code_op2_vxvy(0x90,2);
            break;
        case eBSET: // BSET VX, P  met à 1 le bit P de VX
            code_op2_vxp(0x90,0xA);
            break;
        case eBCLR: // BCLR VX, P  met à 0 le bit P de VX
            code_op2_vxp(0x90,0xB);
            break;
        case eBINV: // BINV VX, P  inverse le bit P de VX
            code_op2_vxp(0x90,0xC);
           break;
        case eBTSS: // BTSS VX, P  saut conditionnel si bit P==1
            code_op2_vxp(0x90,0xC);
            break;
        case eBTSC: // BTSC VX, P  saut conditionnel si bit P==0
            code_op2_vxp(0x90,0xE);
            break;
        case ePIXI: // PIXI VX, VY inverse le pixel à la coordonnée VX,VY
            code_op2_vxvy(0x90,3);
            break;        
        case eGPIX: // GPIX VX, VY met la couleur du pixel à la position
                    // VX,VY dans V0
            code_op2_vxvy(0x90,0xF);
            break; 
        case eRND: // RND VX,P   valeuur aléatoire dans VX masqué par P bits
            code_op2_vxp(0xC0,0);
            break;
        // ***  opérations à 3 arguments *****
        case eDRW: // DRW VX,VY,N  dessine le sprite pointé par I
                   // à la position VX,VY  N indique le nombre pixels vertical 0=16
            code_draw();
            break;
            
    }//switch(n)
}

static void process_directive(int n){
    switch(n){
        case eEQU:
            equate();
            break;
        case eDB:
            data_byte();
            break;
        case eDW:
            data_word();
            break;
        case eASCII:
            data_ascii();
            break;
        case eDEF:
            define();
            break;
        case eSPACE:
            reserve_space();
            break;
        case eEND:
            expect_none();
            reader->eof=true;
            break;
    }//switch
}

static void process_line();

static void new_label(){
    node_t *n;
    char *name;
    
    if ((n=search_list(tok_value,label_list))){
        throw(eDUPLICATE);
        //return;
    }
    name=dalloc(strlen(tok_value)+1);
    if (name){
        strcpy(name,tok_value);
        label_list=add_label(name,pc);
        process_line();
    }else{
        throw(eMEMORY);
    }
    
}

static void process_line(){
    int n;
    
    tok_count=0;
    next_token();
    switch(tok_id){
        case eLABEL:
            new_label();
            break;
        case eSYMBOL:
            if ((n= search_word(tok_value,mnemonics,KW_COUNT))<KW_COUNT){
                process_mnemonic(n);
            }else if ((n=search_word(tok_value,directives,DIR_COUNT))<DIR_COUNT){
                process_directive(n);
            }else{
                throw(eSYNTAX);
            }
            break;
        case eNONE:
            break;
        default:
            throw(eSYNTAX);
            break;
    }//switch
    //if (!(reader->eof || (tok_id==eNONE))) throw(eSYNTAX);
}

static void add_predefined(){
	symbol_list=add_symbol("UP",16); // joystick
	symbol_list=add_symbol("DOWN",8);
	symbol_list=add_symbol("LEFT",4);
	symbol_list=add_symbol("RIGHT",2);
	symbol_list=add_symbol("FIRE_BTN",1);
	define_list=add_define("W","V0"); // working register
	define_list=add_define("C","VF"); // carry register
    // nécessaire pour éviter que preprocess_addr() ne
    // prenne ces symboles pour des étiquettes
    define_list=add_define("DT","DT");
    define_list=add_define("ST","ST");
    define_list=add_define("I","I");
}

static void create_label_file(const char *src){
    char fname[40];
    int i=0;
    struct fat_file_struct *fh;
    struct fat_dir_entry_struct dir_entry;
    node_t *n;
    
    while (*src!='.'){
        fname[i++]=*src++;
    }
    fname[i++]='.';
    fname[i++]='l';
    fname[i++]='b';
    fname[i++]='l';
    fname[i]=0;
    if (fs_find_file(fname,&dir_entry)==eERR_NOTFILE){
        fs_create_file(fname);
    } 
    if ((fh=fs_open_file(fname))){
        //new_line();
        n=label_list;
        while (n){
            memset(fname,0,40);
            memcpy(fname,n->name,strlen(n->name));
            for(i=strlen(n->name);i<20;i++)fname[i]=' ';
            sprintf(&fname[i],"%d\n",n->value);
            //print(fname);
            fs_write_file(fh,(uint8_t*)fname,strlen(fname));
            n=n->next;
        }
        fs_close_file(fh);
    }
}//f()

static void fix_forward_ref(){
	node_t *fwd;
	node_t *lbl;
	
    print("fixing forward reference\n");
	fwd=forward_list;
	while (fwd){
		lbl=search_label(fwd->name);
		if (lbl){ 
			binary[fwd->pc] |= (lbl->addr>>9)&0xf;
			binary[fwd->pc+1] = (lbl->addr>>1)&0xff;
            //new_line();
		}else{
			print(fwd->name);
            new_line();
			throw(eNOTREF);
            //break;
		}
		fwd=fwd->next;
	}//while
}

void pv16asm(const char *src, const char *bin){
    struct fat_file_struct *fh;
    struct fat_dir_entry_struct dir_entry;
    uint8_t *mark=dmark();
    
    new_line();
    print("PV16SOG assembler\n");
    binary=dalloc(PROG_SPACE);
    line=dalloc(LINE_LEN_MAX+1);
    tok_value=dalloc(LINE_LEN_MAX+1);
    reader=dalloc(sizeof(reader_t));
    if (!binary || !line || !tok_value || !reader){
        error(eERR_MEM);
        goto exit_asm;
    }
    sram_clear();
    if (!(reader->fsize=fs_load_file(src,0,SRAM_SIZE-1))){
        prompt("empty file, any key...","*");
        goto exit_asm;
    }
	pc=0;
	line_no=0;
    label_list=NULL;
    symbol_list=NULL;
    forward_list=NULL;
    define_list=NULL;
	add_predefined();
	line[LINE_LEN_MAX]=0;
    print("assembling "); 
    print(src); 
    put_char(' ');
    print_int(reader->fsize,0);
    new_line();
    unget_token=false;
    if (!setjmp(failed)){
        while (!reader->eof && (pc<(PROG_SPACE-1)) && (kbd_get_key()!=ESC)){
            process_line();
        }
    }else{
        goto exit_asm;
    }
	if ((pc>(PROG_SPACE-2)) && !reader->eof){
        throw(eMEMORY);
    }
	fix_forward_ref();
    if (fs_find_file(bin,&dir_entry)&& (fs_last_error()==eERR_NOTFILE)){
        fs_create_file(bin);
    }
    if (!(fh=fs_open_file(bin))){
        throw(eBINFILE);
    }else{ 
        fs_write_file(fh,binary,pc);
        fs_close_file(fh);
    }
    create_label_file(src);
	print("lines read: ");
    print_int(line_no,0);
    new_line();
    print("binary size: ");
    print_int(pc,0);
exit_asm:
    dfree(mark);
	return;
}//f()
