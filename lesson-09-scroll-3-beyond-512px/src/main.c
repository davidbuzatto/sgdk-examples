/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 09 - "Scroll (3) - Beyond 512px" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

#include "gfx.h"
#include "sprite.h"

// Sonic animations
#define ANIM_STAND  0
#define ANIM_RUN    3

// function declarations
static void handleInput( void );
static void updateCamera( void );
static void updatePhysics( void );
static void showDebug( void );

// d-pad input state
s16 xOrder;

// scroll state
s16 pixelCount    = 0;  // detects tile boundaries; resets every 8 pixels (0..7)
s16 offset        = 0;  // scroll in pixels, 0..1023
s16 columnToUpdate = 0; // scroll in tiles, 0..127

// tile counter in VRAM
u16 ind;

// Sonic sprite
Sprite *sonicSprite;
s16 sonicX;
s16 sonicY;

int main( bool hard ) {

    SYS_disableInts();

    VDP_setScreenWidth320();
    VDP_setScreenHeight224();

    SPR_init();

    // load background
    PAL_setPalette( PAL0, bg_image.palette->data, CPU );
    ind = TILE_USER_INDEX;
    VDP_drawImageEx( BG_B, &bg_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bg_image.tileset->numTile;

    // load Sonic sprite
    PAL_setPalette( PAL1, sonic_sprite.palette->data, CPU );
    sonicX = sonicY = 160;
    sonicSprite = SPR_addSprite( &sonic_sprite, sonicX, sonicY, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ) );

    SYS_enableInts();

    // horizontal scroll by plane
    VDP_setScrollingMode( HSCROLL_PLANE, VSCROLL_PLANE );

    while ( TRUE ) {

        handleInput();
        updatePhysics();
        showDebug();
        SPR_update();

        SYS_doVBlankProcess();

    }

    return 0;

}

// displays scroll debug values on screen
static void showDebug( void ) {

    char buf[32];

    VDP_setTextPalette( PAL0 );

    sprintf( buf, "%4d", offset );
    VDP_drawText( "offset          :", 12, 12 );
    VDP_drawText( buf, 30, 12 );

    sprintf( buf, "%4d", pixelCount );
    VDP_drawText( "pixelCount      :", 12, 14 );
    VDP_drawText( buf, 30, 14 );

    sprintf( buf, "%4d", columnToUpdate );
    VDP_drawText( "columnToUpdate  :", 12, 16 );
    VDP_drawText( buf, 30, 16 );

}

// applies the scroll offset and updates the tile column that is about to scroll into view
static void updatePhysics( void ) {

    if ( xOrder > 0 ) { // RIGHT pressed
        offset++;
        pixelCount++;
        if ( pixelCount > 7 ) { pixelCount = 0; } // 0..7 = 8 pixels = 1 tile
        SPR_setAnim( sonicSprite, ANIM_RUN );
        SPR_setHFlip( sonicSprite, FALSE );
    } else if ( xOrder < 0 ) { // LEFT pressed
        offset--;
        pixelCount--;
        if ( pixelCount < -7 ) { pixelCount = 0; }
        SPR_setAnim( sonicSprite, ANIM_RUN );
        SPR_setHFlip( sonicSprite, TRUE );
    } else {
        SPR_setAnim( sonicSprite, ANIM_STAND );
    }

    updateCamera();

}

static void updateCamera( void ) {

    // wrap around when the scroll exceeds the image width
    if ( offset > 1023 ) { offset = 0; }
    // if ( offset < 0 ) { offset = 184; }

    // update only the tile column that is about to scroll into the visible area,
    // just BEFORE it becomes visible (non-visible -> visible transition)
    if ( pixelCount == 0 ) {
        // 320 = screen width in pixels
        // >> 3 converts pixels to tiles (divide by 8)
        // + 23 points to the correct upcoming column
        // & 127 clamps to the range 0..127
        columnToUpdate = ( ( ( offset + 320 ) >> 3 ) + 23 ) & 127;
        VDP_setMapEx(
            BG_B, bg_image.tilemap,
            TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX ),
            columnToUpdate, 0, columnToUpdate, 0, 1, 28
        );
    }

    if ( pixelCount == -1 ) {
        // + 88 points to the correct column when scrolling left
        columnToUpdate = ( ( ( offset + 320 ) >> 3 ) + 88 ) & 127;
        VDP_setMapEx(
            BG_B, bg_image.tilemap,
            TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX ),
            columnToUpdate, 0, columnToUpdate, 0, 1, 28
        );
    }

    VDP_setHorizontalScroll( BG_B, -offset );

}

static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        xOrder = -1;
    } else if ( value & BUTTON_RIGHT ) {
        xOrder = +1;
    } else {
        xOrder = 0;
    }

}
