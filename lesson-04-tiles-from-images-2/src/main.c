/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 04 - "Tiles from Images (2)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include <resources.h>

int main( bool hard ) {

    // tile counter in VRAM
    u16 ind;

    // load the image palette and assign it to PAL0
    PAL_setPalette( PAL0, my64tiles.palette->data, CPU );

    // start loading tiles from VRAM position 1 (position 0 is reserved for the background)
    ind = 1;

    // load the image into VRAM and draw it at tile position (3, 3)
    VDP_drawImageEx( BG_B, &my64tiles, TILE_ATTR_FULL( PAL0, 0, 0, 0, ind ), 3, 3, 0, CPU );

    // advance the tile counter to the next free VRAM slot
    ind += my64tiles.tileset->numTile;

    while ( TRUE ) {

        SYS_doVBlankProcess();

    }

    return 0;

}
