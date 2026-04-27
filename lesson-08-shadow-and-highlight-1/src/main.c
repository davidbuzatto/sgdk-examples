/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 08 - "Shadow and Highlight (1)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include "fondos.h"
#include "sprite.h"
#include <sys.h>

// function declarations
static void handleInput( void );
void myJoyHandler( u16 joy, u16 changed, u16 state );
void reset( void );

// Sonic sprite
Sprite *sonicSprite;

// Sonic position on screen
u32 sonicPosx = 64;
u32 sonicPosy = 155;

// horizontal bar sprite (shows the full palette)
Sprite *barSprite;

// bar position on screen
u32 barPosx = 120;
u32 barPosy = 96;

// current demo state
int state = 0;

// tile counter in VRAM
u16 ind;

int main( bool hard ) {

    // set display resolution to 320x224
    VDP_setScreenWidth320();

    // initialize the sprite engine
    SPR_init();

    // load the background palette into PAL0
    PAL_setPalette( PAL0, fondo1.palette->data, CPU );

    // load the background into the VDP
    ind = TILE_USER_INDEX;
    // VDP_drawImageEx( BG_B, &fondo1, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    // ind += fondo1.tileset->numTile;
    VDP_drawImageEx( BG_A, &fondo1, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += fondo1.tileset->numTile;

    // load the Sonic palette into PAL1
    PAL_setPalette( PAL1, sonic_sprite.palette->data, CPU );

    // load the bar palette into PAL2 and PAL3
    PAL_setPalette( PAL2, barra_sprite.palette->data, CPU );
    PAL_setPalette( PAL3, barra_sprite.palette->data, CPU );

    // add the bar sprite
    barSprite = SPR_addSprite( &barra_sprite, barPosx, barPosy, TILE_ATTR( PAL2, FALSE, FALSE, FALSE ) );

    // add the Sonic sprite
    sonicSprite = SPR_addSprite( &sonic_sprite, sonicPosx, sonicPosy, TILE_ATTR( PAL1, FALSE, FALSE, FALSE ) );

    // register the asynchronous joypad handler
    JOY_init();
    JOY_setEventHandler( &myJoyHandler );

    // initial on-screen text
    VDP_drawText( "TEST  0", 1, 0 );
    VDP_drawText( "-Press A to continue-", 1, 27 );

    while ( TRUE ) {

        // read joypad (synchronous) and move the bar
        handleInput();

        // flush sprite updates to the VDP
        SPR_update();

        // sync with the TV vertical blank
        SYS_doVBlankProcess();

    }

    return 0;

}

// synchronous input — moves the bar sprite with the d-pad
static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        SPR_setPosition( barSprite, barPosx--, barPosy );
    }

    if ( value & BUTTON_RIGHT ) {
        SPR_setPosition( barSprite, barPosx++, barPosy );
    }

    if ( value & BUTTON_UP ) {
        SPR_setPosition( barSprite, barPosx, barPosy-- );
    }

    if ( value & BUTTON_DOWN ) {
        SPR_setPosition( barSprite, barPosx, barPosy++ );
    }

}

// asynchronous input — A/B/C cycle through shadow/highlight demo states
void myJoyHandler( u16 joy, u16 changed, u16 state ) {

    if ( joy == JOY_1 ) {

        // STATE 0: shadow/highlight OFF — everything renders normally

        // STATE 01: enable S&H — all sprites without priority appear in SHADOW mode
        if ( state == 0 && ( changed & BUTTON_A ) ) {
            state = 1;
            VDP_drawText( "TEST 01", 1, 0 );
            VDP_drawText( "-Press B to continue-", 1, 27 );
            VDP_setHilightShadow( 1 );
        }

        // STATE 02: Sonic gets high priority (normal), bar stays in shadow
        if ( state == 1 && ( changed & BUTTON_B ) ) {
            state = 2;
            VDP_drawText( "TEST 02", 1, 0 );
            VDP_drawText( "-Press C to continue-", 1, 27 );
            SPR_setPriority( sonicSprite, TRUE );
            SPR_setPriority( barSprite, FALSE );
        }

        // STATE 03: bar gets high priority (highlight), Sonic back in shadow
        if ( state == 2 && ( changed & BUTTON_C ) ) {
            state = 3;
            VDP_drawText( "TEST 03", 1, 0 );
            VDP_drawText( "-Press A to continue-", 1, 27 );
            SPR_setPriority( sonicSprite, FALSE );
            SPR_setPriority( barSprite, TRUE );
        }

        // STATE 04: both sprites without priority; bar switches to PAL3
        if ( state == 3 && ( changed & BUTTON_A ) ) {
            state = 4;
            VDP_drawText( "TEST 04", 1, 0 );
            VDP_drawText( "-Press B to continue-", 1, 27 );
            SPR_setPriority( sonicSprite, FALSE );
            SPR_setPriority( barSprite, FALSE );
            SPR_setPalette( barSprite, PAL3 );
        }

        // STATE 05: bar gets high priority while on PAL3
        if ( state == 4 && ( changed & BUTTON_B ) ) {
            state = 5;
            VDP_drawText( "TEST 05", 1, 0 );
            VDP_drawText( "-Press C to continue-", 1, 27 );
            SPR_setPriority( barSprite, TRUE );
        }

        // STATE 06: redraw background with high priority — background highlights sprites below it
        if ( state == 5 && ( changed & BUTTON_C ) ) {
            state = 6;
            ind = TILE_USER_INDEX;
            VDP_drawImageEx( BG_A, &fondo1, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
            ind += fondo2.tileset->numTile;
            VDP_drawText( "TEST 06", 1, 0 );
            VDP_drawText( "-Press A to continue-", 1, 27 );
            SPR_setPriority( sonicSprite, FALSE );
            SPR_setPriority( barSprite, FALSE );
        }

        // STATE 07: move Sonic to a more visible position
        if ( state == 6 && ( changed & BUTTON_A ) ) {
            state = 7;
            VDP_drawText( "TEST 07", 1, 0 );
            VDP_drawText( "-Press B to continue-", 1, 27 );
            SPR_setPosition( sonicSprite, sonicPosx - 30, sonicPosy );
        }

        // STATE 08: both sprites get high priority (highlight)
        if ( state == 7 && ( changed & BUTTON_B ) ) {
            state = 8;
            VDP_drawText( "TEST 08", 1, 0 );
            VDP_drawText( "-Press C to restart-", 1, 27 );
            SPR_setPriority( sonicSprite, TRUE );
            SPR_setPriority( barSprite, TRUE );
        }

        // RESET
        if ( state == 8 && ( changed & BUTTON_C ) ) {
            reset();
        }

    }

}

void reset( void ) {
    SYS_hardReset();
}
