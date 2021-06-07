/*
 * bitmaps.h
 *
 *  Created on: May 23, 2021
 *      Author: gauth
 */

#ifndef INC_BITMAPS_H_
#define INC_BITMAPS_H_

#include <stdint.h>


//  Structure used to define bitmaps

typedef struct {
    const uint8_t IconWidth;    /* Icon width in pixels */
    uint8_t IconHeight;         /* Icon height in pixels */
    uint8_t IconParts;         /* Icon parts */
    const uint16_t *data;       /* Pointer to data font data array */
} IconDef;


//  Export available bitmaps

extern IconDef Icon_16x16;
extern IconDef Icon_32x9;
extern IconDef Icon_9x10;
extern IconDef Icon_8x8;

#endif /* INC_BITMAPS_H_ */
