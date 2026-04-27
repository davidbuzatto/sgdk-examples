/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 04 - "Tiles from Images (4)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include <resources.h>

// function declarations
static void handleInput( void );

int main( bool hard ) {

    // local copy of all four palettes (64 entries total, 16 per palette)
    // modifying this array does NOT affect the hardware palette directly
    u16 fullPalette[64];

    // tile counter in VRAM
    u16 ind;

    // start loading tiles from VRAM position 1 (position 0 is reserved for the background)
    ind = 1;

    // disable VDP interrupts so we can update VRAM without being interrupted
    // (Vertical, Horizontal and External interrupts are all disabled)
    SYS_disableInts();

    // set display resolution to 320x240
    VDP_setScreenWidth320();

    // set ALL 64 palette entries to black — preparation for a fade-in effect
    // PAL_setColors( start_index, rgb_value, num_colors, transfer_method )
    //   sets 'num_colors' entries starting at 'start_index' to 'rgb_value'
    //   here: colors 0..63 all set to black
    PAL_setColors( 0, (u16 *) palette_black, 64, CPU );

    // load both background images into VRAM using DMA for speed
    VDP_drawImageEx( BG_B, &bgb_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bgb_image.tileset->numTile;

    VDP_drawImageEx( BG_A, &bga_image, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ), 20, 0, FALSE, TRUE );
    ind += bga_image.tileset->numTile;

    // re-enable VDP interrupts
    SYS_enableInts();

    // copy both image palettes into the local array
    memcpy( &fullPalette[0],  bgb_image.palette->data, 16 * 2 );
    memcpy( &fullPalette[16], bga_image.palette->data, 16 * 2 );

    // FADE IN
    // PAL_fadeIn( from_color, to_color, target_palette, num_frames, async )
    //   performs a fade from the current (all-black) palette to 'target_palette'
    //   over 'num_frames' frames. At 60 fps (NTSC) this is ~1.67 seconds.
    //   from_color=0, to_color=63 — all 64 entries affected.
    //   async=FALSE — execution blocks until the fade completes.
    //   async=TRUE  — execution continues while the fade runs in the background.
    PAL_fadeIn( 0, 63, fullPalette, 100, FALSE );

    while ( TRUE ) {

        // read controls — called once per frame
        handleInput();

        SYS_doVBlankProcess();

    }

    return 0;

}

static void handleInput( void ) {

    // store the current joypad state
    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        VDP_drawText( "LEFT", 8, 10 );
    }

    if ( value & BUTTON_RIGHT ) {
        VDP_drawText( "RIGHT", 20, 10 );
    }

    if ( value & BUTTON_UP ) {
        VDP_drawText( "UP", 16, 8 );
    }

    if ( value & BUTTON_DOWN ) {
        VDP_drawText( "DOWN", 16, 12 );
    }

    if ( value & BUTTON_A ) {
        VDP_drawText( "BUTTON A", 6, 18 );
    }

    if ( value & BUTTON_B ) {
        VDP_drawText( "BUTTON B", 14, 18 );
    }

    if ( value & BUTTON_C ) {
        VDP_drawText( "BUTTON C", 22, 18 );
    }

    if ( value & BUTTON_X ) {
        VDP_drawText( "BUTTON X", 6, 20 );
    }

    if ( value & BUTTON_Y ) {
        VDP_drawText( "BUTTON Y", 14, 20 );
    }

    if ( value & BUTTON_Z ) {
        VDP_drawText( "BUTTON Z", 22, 20 );
    }

    if ( value & BUTTON_START ) {
        VDP_drawText( "START", 10, 16 );
    }

    if ( value & BUTTON_MODE ) {
        VDP_drawText( "MODE", 20, 16 );
    }

    // if no button is pressed, clear the area below the title
    // VDP_clearTextArea() works in tiles
    if ( !( value & BUTTON_UP )    && !( value & BUTTON_DOWN )  &&
         !( value & BUTTON_LEFT )  && !( value & BUTTON_RIGHT ) &&
         !( value & BUTTON_A )     && !( value & BUTTON_B )     &&
         !( value & BUTTON_C )     && !( value & BUTTON_X )     &&
         !( value & BUTTON_Y )     && !( value & BUTTON_Z )     &&
         !( value & BUTTON_MODE )  && !( value & BUTTON_START ) ) {
        VDP_clearTextArea( 0, 7, 40, 28 );
    }

}
