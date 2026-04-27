/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 03 - "Tiles and Planes (2)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

// these constants represent two tiles at VRAM positions 1 and 2
#define MY_TILE1 1
#define MY_TILE2 2

/*
  A tile is by definition 8x8 pixels:
  - Each pixel can have a color from a 16-color palette (0..F), i.e. 4bpp
  - 4bpp = 4 bits per pixel = 2^4 = 16 colors total
  A tile can therefore be represented as an 8x8 color matrix.
  - A tile can only use one palette (but any palette)
  - A tile can be drawn as-is, mirrored (horizontally) and/or flipped (vertically)
  - A tile is NOT erased between frames — the VDP keeps it on screen automatically
*/

// two arrays of 8 u32 values, each holding 8 pixels (4 bits each) = 1 tile (8x8)
const u32 tile1[8] = {
    0x00000001,
    0x00000012,
    0x00000123,
    0x00001234,
    0x00012344,
    0x00123456,
    0x01234567,
    0x12345678
};

const u32 tile2[8] = {
    0xAAAAAAAA,
    0xABBBBBBA,
    0xABBBBBBA,
    0xACCCCCCA,
    0xADDDDDDA,
    0xACCCCCCA,
    0xABBBBBBA,
    0xAAAAAAAA
};

// function declarations
void reset( void );

/*
  Steps to draw a tile:
  1. Load the tile into VRAM
  2. Load or assign its palette
  3. Draw the tile (choose plane and position).
     Recommended: use TILE_ATTR_FULL() to assign palette, flipping, etc.
*/

int main( bool hard ) {

    /* Load tiles into VRAM.
       The third parameter is the number of tiles to load (1 at a time here).
       The fourth parameter is the transfer mode: 0 = CPU -> VRAM (no DMA). */

    // on-screen help
    // VDP_drawText( "text", x, y );
    VDP_drawText( "Lesson 3: TILES AND PLANES", 1, 1 );
    VDP_drawText( "Example 1: VDP_setTileMapXY()    ", 1, 3 );
    VDP_drawText( "2 tiles loaded at (5,5) and (7,4)", 1, 4 );
    VDP_drawText( "no palette assigned, uses PAL0   ", 1, 5 );
    VDP_drawText( "from SGDK — the gray palette     ", 1, 6 );

    VDP_loadTileData( (const u32 *) tile1, MY_TILE1, 1, 0 );
    VDP_loadTileData( (const u32 *) tile2, MY_TILE2, 1, 0 );

    // tiles are now in VRAM but not yet drawn to the screen

    while ( TRUE ) {

        // variable to store joypad input
        u16 value;

        // Example 1
        // draw tiles at VRAM positions MY_TILE1 and MY_TILE2 on plane A,
        // at screen positions (10, 10) and (12, 10). No palette specified
        // so SGDK uses PAL0.
        // VDP_setTileMapXY( plane (A/B/WINDOW), vram-index, x, y )
        //   — (x, y) are in tiles, not pixels
        VDP_setTileMapXY( BG_A, MY_TILE1, 10, 10 );
        VDP_setTileMapXY( BG_A, MY_TILE2, 12, 10 );

        VDP_drawText( "Press -A- to continue", 1, 8 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_A ) {
                break;
            }
        }

        // Example 2
        // fill rectangular areas with tiles — grass and ground
        VDP_drawText( "Example 2: VDP_fillTileMapRect() ", 1, 3 );
        VDP_drawText( "fills an area with a specific tile", 1, 4 );
        VDP_drawText( "uses PAL1 and PAL2               ", 1, 5 );
        VDP_drawText( "from SGDK — reds and greens      ", 1, 6 );

        // VDP_fillTileMapRect( plane, tile or attributes, x, y, width, height )
        VDP_fillTileMapRect( BG_A, TILE_ATTR_FULL( PAL2, 0, 0, 0, MY_TILE1 ), 0, 21, 40, 1 );
        VDP_fillTileMapRect( BG_A, TILE_ATTR_FULL( PAL1, 0, 0, 0, MY_TILE2 ), 0, 22, 40, 7 );

        VDP_drawText( "Press -B- to continue", 1, 8 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_B ) {
                break;
            }
        }

        VDP_drawText( "We used TILE_ATTR_FULL() to       ", 1, 3 );
        VDP_drawText( "specify the palette. The next     ", 1, 4 );
        VDP_drawText( "example shows other uses, such as ", 1, 5 );
        VDP_drawText( "flipping and mirroring tiles.     ", 1, 6 );

        VDP_drawText( "Press -C- to continue", 1, 8 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_C ) {
                break;
            }
        }

        VDP_drawText( "Note that tiles drawn in Example 1", 1, 3 );
        VDP_drawText( "were not erased — by default the  ", 1, 4 );
        VDP_drawText( "VDP keeps drawn tiles on screen   ", 1, 5 );
        VDP_drawText( "until explicitly cleared.         ", 1, 6 );

        VDP_drawText( "Press -A- to continue", 1, 8 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_A ) {
                break;
            }
        }

        // Example 3
        // draw one tile in all four flip combinations, each in a different area and palette
        //   area 1: tile as-is
        //   area 2: horizontally flipped
        //   area 3: vertically flipped
        //   area 4: both horizontally and vertically flipped
        VDP_drawText( "Example 3: draw 4 AREAS            ", 1, 3 );
        VDP_drawText( "area 1 (gray): tile as-is          ", 1, 4 );
        VDP_drawText( "area 2 (red ): vertically flipped  ", 1, 5 );
        VDP_drawText( "area 3 (grn ): horizontally flipped", 1, 6 );
        VDP_drawText( "area 4 (blue): both flips          ", 1, 7 );

        // TILE_ATTR_FULL( palette, priority, vflip, hflip, tile )
        VDP_fillTileMapRect( BG_A, TILE_ATTR_FULL( PAL0, 0, 0, 0, MY_TILE1 ),  0, 10, 20, 9 );
        VDP_fillTileMapRect( BG_A, TILE_ATTR_FULL( PAL1, 0, 1, 0, MY_TILE1 ), 20, 10, 20, 9 );
        VDP_fillTileMapRect( BG_A, TILE_ATTR_FULL( PAL2, 0, 0, 1, MY_TILE1 ),  0, 19, 20, 9 );
        VDP_fillTileMapRect( BG_A, TILE_ATTR_FULL( PAL3, 0, 1, 1, MY_TILE1 ), 20, 19, 20, 9 );

        VDP_drawText( "Press -B- to continue", 1, 8 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_B ) {
                break;
            }
        }

        // Example 4 — background color change via palette index
        VDP_drawText( "Example 4: Color change            ", 1, 3 );
        VDP_drawText( " Change the background color by    ", 1, 4 );
        VDP_drawText( " assigning a different palette     ", 1, 5 );
        VDP_drawText( " color index to the background.    ", 1, 6 );
        VDP_drawText( "                                   ", 1, 7 );

        VDP_setBackgroundColor( 20 );

        VDP_drawText( "Press -C- to continue", 1, 8 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_C ) {
                break;
            }
        }

        // Example 5 — background color change by modifying the palette entry directly
        VDP_drawText( "Example 5: Color change            ", 1, 3 );
        VDP_drawText( " Change the background color by    ", 1, 4 );
        VDP_drawText( " modifying the palette entry at    ", 1, 5 );
        VDP_drawText( " index 20 directly (no index swap).", 1, 6 );
        VDP_drawText( "                                   ", 1, 7 );

        PAL_setColor( 20, RGB24_TO_VDPCOLOR( 0x0098e5 ) );

        VDP_drawText( "Press -A- to continue", 1, 8 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_A ) {
                break;
            }
        }

        // soft reset
        reset();

        SYS_doVBlankProcess();

    }

    return 0;

}

void reset( void ) {
    asm( "move   #0x2700,%sr\n\t"
         "move.l (0),%a7\n\t"
         "move.l (4),%a0\n\t"
         "jmp    (%a0)" );
}
