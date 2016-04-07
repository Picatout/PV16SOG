/* 
 * File:   asm.h
 * Author: jacques
 * Description: assembleur pour pv16vm
 * Created on 3 août 2015, 16:03
 */

#ifndef ASM_H
#define	ASM_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdint.h>


    uint8_t vm_asm(const char *name);

#ifdef	__cplusplus
}
#endif

#endif	/* ASM_H */

