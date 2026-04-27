/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 07 - "Deconstructing Scenarios" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include "gfx.h"

int main( bool hard ) {

    u16 ind;
    u16 counter;

    SYS_disableInts();

    VDP_setScreenWidth320();

    PAL_setPalette( PAL0, bga_image.palette->data, CPU );

    ind = TILE_USER_INDEX;
    VDP_drawImageEx( BG_A, &bga_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bga_image.tileset->numTile;

    SYS_enableInts();

    counter = 0;

    while ( TRUE ) {

        // at 60 fps each second increments the counter by 60
        counter++;

        // sky color transition (blue -> orange -> red -> black)
        if ( counter > 60 * 1 ) { PAL_setColor( 4, RGB24_TO_VDPCOLOR( 0xFF6600 ) ); }
        if ( counter > 60 * 2 ) { PAL_setColor( 4, RGB24_TO_VDPCOLOR( 0xCC5200 ) ); }
        if ( counter > 60 * 3 ) { PAL_setColor( 4, RGB24_TO_VDPCOLOR( 0x993D00 ) ); }
        if ( counter > 60 * 4 ) { PAL_setColor( 4, RGB24_TO_VDPCOLOR( 0x662900 ) ); }
        if ( counter > 60 * 5 ) { PAL_setColor( 4, RGB24_TO_VDPCOLOR( 0x000000 ) ); }

        // castle color transition (red -> dark red)
        if ( counter > 60 * 3 ) { PAL_setColor( 2,  RGB24_TO_VDPCOLOR( 0x802000 ) ); }
        if ( counter > 60 * 4 ) { PAL_setColor( 12, RGB24_TO_VDPCOLOR( 0x661A00 ) ); }

        // grass color transition (green -> brown)
        if ( counter > 60 * 1 ) { PAL_setColor( 8, RGB24_TO_VDPCOLOR( 0x999900 ) ); }
        if ( counter > 60 * 2 ) { PAL_setColor( 1, RGB24_TO_VDPCOLOR( 0x997300 ) ); }

        SYS_doVBlankProcess();

    }

    return 0;

}
