/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 05 - "Sprites (1)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include "sprites.h"

// function declarations
static void handleInput( void );
static void drawPosition( void );

// sprite pointers
Sprite *squareSprite;
Sprite *sonicSprite;
Sprite *currentSprite;

// current sprite position on screen
int posx;
int posy;

int main( bool hard ) {

    // initial sprite position
    posx = 0;
    posy = 0;

    // set display resolution to 320x224
    VDP_setScreenWidth320();

    // initialize the sprite engine with default parameters
    SPR_init();

    // load the sprite palette into the second system palette slot
    // (using PAL0 would make the coordinate text invisible — it renders in black)
    PAL_setPalette( PAL1, sprite_8x8.palette->data, CPU );

    // create both sprites at the initial position
    squareSprite  = SPR_addSprite( &sprite_8x8,    posx, posy, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ) );
    sonicSprite   = SPR_addSprite( &sprite_sonic,   posx, posy, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ) );

    // start with the square sprite active
    currentSprite = squareSprite;

    while ( TRUE ) {

        // read joypad and update sprite position
        handleInput();

        // display the current position on screen
        drawPosition();

        // flush sprite updates to the VDP
        SPR_update();

        // sync with the TV vertical blank
        SYS_doVBlankProcess();

    }

    return 0;

}

// reads joypad input and moves the active sprite;
// also switches between the two sprites on A/B
static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        SPR_setPosition( currentSprite, posx--, posy );
    }

    if ( value & BUTTON_RIGHT ) {
        SPR_setPosition( currentSprite, posx++, posy );
    }

    if ( value & BUTTON_UP ) {
        SPR_setPosition( currentSprite, posx, posy-- );
    }

    if ( value & BUTTON_DOWN ) {
        SPR_setPosition( currentSprite, posx, posy++ );
    }

    // switch to the square sprite
    if ( ( value & BUTTON_A ) && ( currentSprite == sonicSprite ) ) {
        currentSprite = squareSprite;
        SPR_setPosition( currentSprite, posx, posy ); // force immediate screen update
    }

    // switch to the Sonic sprite
    if ( ( value & BUTTON_B ) && ( currentSprite == squareSprite ) ) {
        currentSprite = sonicSprite;
        SPR_setPosition( currentSprite, posx, posy );
    }

}

// writes the current sprite position to the screen
// sprintf converts numeric values to strings; %4d right-aligns to handle
// the transition from negative to positive numbers cleanly
static void drawPosition( void ) {

    char buf[32];

    sprintf( buf, "x:  %4d", posx );
    VDP_drawText( buf, 2, 23 );

    sprintf( buf, "y:  %4d", posy );
    VDP_drawText( buf, 2, 24 );

}
