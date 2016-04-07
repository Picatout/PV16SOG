/*
    Description: assembleur pour pv16vm
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Hardware/hardware.h"
#include "filesys.h"
#include "Hardware/spi_ram.h"
#include "Hardware/spi.h"
#include "text.h"

#define FIRST_NODE 8192

typedef struct data_node{
	char name[9];
	union {
            uint16_t addr;
            uint16_t pc;
            uint16_t value;
	};
}node_t;

typedef enum token {eNONE,eSYMBOL,eLABEL,eNUMBER,eSTRING,eADDOP,eMULOP,
                    eCOMMA,eLPAREN,eRPAREN,eLBRACKET,eRBRACKET,eDOT} token_t;

token_t tok_id;
char *tok_value;
node_t node;

static bool asm_init(){

    tok_value=malloc(64);
    if (!tok_value) return false;
    return true;
}//f()

uint8_t vm_asm(const char *name){

    if (!asm_init()){
        print_error("memoire insufisante");
        return 0;
    }
    return 1;
}//f();

