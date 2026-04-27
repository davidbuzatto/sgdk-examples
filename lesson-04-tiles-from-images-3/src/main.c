/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 04 - "Tiles from Images (3)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include <resources.h>

int main( bool hard ) {

    // tile counter in VRAM
    u16 ind;

    // load each image's palette into its corresponding palette slot
    PAL_setPalette( PAL0, block.palette->data, CPU );
    PAL_setPalette( PAL1, block2.palette->data, CPU );

    // start loading tiles from VRAM position 1 (position 0 is reserved for the background)
    ind = 1;

    // load both images into VRAM and advance the tile counter after each
    VDP_drawImageEx( BG_A, &block,  TILE_ATTR_FULL( PAL0, 0, 0, 0, ind ),  3,  3, 1, CPU );
    ind += block.tileset->numTile;

    VDP_drawImageEx( BG_B, &block2, TILE_ATTR_FULL( PAL1, 0, 0, 0, ind ), 15, 15, 1, CPU );
    ind += block2.tileset->numTile;

    while ( TRUE ) {

        SYS_doVBlankProcess();

    }

    return 0;

}
