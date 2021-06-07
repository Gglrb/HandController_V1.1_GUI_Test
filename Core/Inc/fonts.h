#ifndef _FONTS_H
#define _FONTS_H

#include <stdint.h>


//  Structure used to define bitmaps

typedef struct {
    const uint8_t FontWidth;    /* Font width in pixels */
    uint8_t FontHeight;         /* Font height in pixels */
    const uint16_t *data;       /* Pointer to data font data array */
} FontDef;


//  Export available bitmaps

extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

#endif  // _FONTS_H
