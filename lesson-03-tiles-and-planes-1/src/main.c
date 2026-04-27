/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 03 - "Tiles and Planes (1)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

#define MY_TILE 1

const u32 tile[8] = {
    0x44444421,
    0x44442211,
    0x44422110,
    0x44221100,
    0x42211000,
    0x42110000,
    0x21100000,
    0x11000000
};

int main( bool hard ) {

    // load the tile into VRAM
    VDP_loadTileData( (const u32 *) tile, MY_TILE, 1, 0 );

    // draw the tile on plane A at position (1, 5) using the default palette (PAL0)
    VDP_setTileMapXY( BG_A, MY_TILE, 1, 5 );

    // draw the tile using TILE_ATTR_FULL():
    //   first  param: palette — PAL2 = green palette
    //   second param: priority — 0 = low priority
    //   third  param: vertical flip — 1 = vflip
    //   fourth param: horizontal flip — 0 = no hflip
    //   fifth  param: tile index in VRAM
    VDP_setTileMapXY( BG_A, TILE_ATTR_FULL( PAL2, 0, 0, 0, MY_TILE ), 3, 5 );

    // draw the tile twice: once on plane B with PAL0 (grays), once on plane A with PAL1 (reds)
    VDP_setTileMapXY( BG_B, TILE_ATTR_FULL( PAL0, 0, 0, 0, MY_TILE ), 5, 5 );
    VDP_setTileMapXY( BG_A, TILE_ATTR_FULL( PAL1, 0, 0, 0, MY_TILE ), 5, 5 );

    // same as above, but the plane B tile has high priority — it renders on top
    VDP_setTileMapXY( BG_B, TILE_ATTR_FULL( PAL0, 1, 0, 0, MY_TILE ), 7, 5 );
    VDP_setTileMapXY( BG_A, TILE_ATTR_FULL( PAL1, 0, 0, 0, MY_TILE ), 7, 5 );

    // draw the tile flipped horizontally, vertically, and both at once
    VDP_setTileMapXY( BG_A, TILE_ATTR_FULL( PAL0, 1, 0, 1, MY_TILE ), 3, 8 );
    VDP_setTileMapXY( BG_A, TILE_ATTR_FULL( PAL0, 1, 1, 0, MY_TILE ), 5, 8 );
    VDP_setTileMapXY( BG_A, TILE_ATTR_FULL( PAL0, 1, 1, 1, MY_TILE ), 7, 8 );

    while ( TRUE ) {

        SYS_doVBlankProcess();

    }

    return 0;

}
