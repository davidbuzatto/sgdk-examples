/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 05 - "Sprites (2) Animated" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

#include "backgrounds.h"
#include "sprites.h"

#define ANIM_STAND   0
#define ANIM_WAIT    1
#define ANIM_WALK    2
#define ANIM_RUN     3
#define ANIM_BRAKE   4
#define ANIM_UP      5
#define ANIM_CROUCH  6
#define ANIM_ROLL    7

// function declarations
static void handleInput( void );

// sprite pointer
Sprite *sonicSprite;

// sprite position on screen
u32 posx = 64;
u32 posy = 155;

int main( bool hard ) {

    // tile counter in VRAM
    u16 ind;

    // set display resolution to 320x224
    VDP_setScreenWidth320();

    // BACKGROUND

    // load the background palette into PAL1
    PAL_setPalette( PAL1, bg2.palette->data, CPU );

    // start loading tiles from VRAM position 1
    ind = 1;

    // load the background image into plane B
    VDP_drawImageEx( BG_B, &bg2, TILE_ATTR_FULL( PAL1, 1, 0, 0, ind ), 0, 0, 1, CPU );
    ind += bg2.tileset->numTile;

    // SPRITES

    // initialize the sprite engine
    SPR_init();

    // load the Sonic palette into PAL0
    PAL_setPalette( PAL0, sonic_sprite.palette->data, CPU );

    // add the Sonic sprite
    sonicSprite = SPR_addSprite( &sonic_sprite, posx, posy, TILE_ATTR( PAL0, TRUE, FALSE, FALSE ) );

    // ON-SCREEN HELP (drawn on plane A)

    // switch to a palette that has white at index 15 so text is visible
    VDP_setTextPalette( PAL1 );

    VDP_drawText( "Press A  -  Sonic WITHOUT priority    ", 1, 0 );
    VDP_drawText( "Press B  -  Sonic WITH priority       ", 1, 1 );

    while ( TRUE ) {

        // read joypad and update sprite
        handleInput();

        // flush sprite updates to the VDP
        SPR_update();

        // sync with the TV vertical blank
        SYS_doVBlankProcess();

    }

    return 0;

}

// reads joypad input, moves Sonic left/right, flips the sprite accordingly,
// and toggles sprite priority with A/B
static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        SPR_setPosition( sonicSprite, posx--, posy );
        SPR_setHFlip( sonicSprite, TRUE );
        SPR_setAnim( sonicSprite, ANIM_RUN );
        // SPR_setVFlip( sonicSprite, TRUE );
    }

    if ( value & BUTTON_RIGHT ) {
        SPR_setPosition( sonicSprite, posx++, posy );
        SPR_setHFlip( sonicSprite, FALSE );
        SPR_setAnim( sonicSprite, ANIM_RUN );
        // SPR_setVFlip( sonicSprite, FALSE );
    }

    if ( value & BUTTON_A ) {
        SPR_setPriority( sonicSprite, FALSE );
    }

    if ( value & BUTTON_B ) {
        SPR_setPriority( sonicSprite, TRUE );
    }

    // no horizontal input — return to stand animation
    if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) ) {
        SPR_setAnim( sonicSprite, ANIM_STAND );
    }

}
