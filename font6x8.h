#ifndef FONT6X8_H_
#define FONT6X8_H_

#ifdef	__cplusplus
extern "C" {
#endif

#define FONT_SIZE   (101)
#define CHAR_HEIGHT (8)
#define CHAR_WIDTH  (6)

#ifndef __asm    
extern __eds__  const uint8_t _FLASH_CONST font_6x8[FONT_SIZE*CHAR_HEIGHT];
#endif //~__asm

#ifdef	__cplusplus
}
#endif

#endif //FONT6X8_H_
