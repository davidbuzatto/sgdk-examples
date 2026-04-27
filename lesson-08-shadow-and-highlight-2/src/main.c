/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 08 - "Shadow and Highlight (2)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include "fondos.h"
#include "sprite.h"

// circle sprite animation indices
#define CIRCLE14  0
#define CIRCLE15  1

// function declarations
static void handleInput( void );
void myJoyHandler( u16 joy, u16 changed, u16 state );
void drawHelp( void );

// sprites
Sprite *sonicSprite;
Sprite *targetSprite;

// Sonic position on screen
u32 sonicPosx = 64;
u32 sonicPosy = 155;

// target (circle) position on screen
u32 targetPosx = 120;
u32 targetPosy = 96;

// shadow/highlight toggle states
int shActive      = 0; // S&H active (1) or off (0)
int shPalette     = 2; // circle palette: PAL2 (2) or PAL3 (3)
int shPriority    = 0; // circle sprite priority: high (1) or low (0)
int shColor       = 14; // circle drawn with color index 14 (14) or 15 (15)
int shBgPriority  = 0; // background priority: high (1) or low (0)

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
    VDP_drawImageEx( BG_A, &fondo1, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += fondo1.tileset->numTile;

    // load the Sonic palette into PAL1
    PAL_setPalette( PAL1, sonic_sprite.palette->data, CPU );

    // load the circle palette into PAL2 and PAL3
    PAL_setPalette( PAL2, circulo_sprite.palette->data, CPU );
    PAL_setPalette( PAL3, circulo_sprite.palette->data, CPU );

    // add the circle sprite
    targetSprite = SPR_addSprite( &circulo_sprite, targetPosx, targetPosy, TILE_ATTR( PAL2, FALSE, FALSE, FALSE ) );
    SPR_setAnim( targetSprite, CIRCLE14 );

    // add the Sonic sprite
    sonicSprite = SPR_addSprite( &sonic_sprite, sonicPosx, sonicPosy, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ) );

    // register the asynchronous joypad handler
    JOY_init();
    JOY_setEventHandler( &myJoyHandler );

    // initial on-screen help
    VDP_drawText( "S&H: OFF                                ", 0, 0 );
    VDP_drawText( "CIRCLE PALETTE: PAL2                    ", 0, 1 );
    VDP_drawText( "Priority: NO                            ", 0, 2 );
    VDP_drawText( "Circle color: 14                        ", 0, 3 );
    VDP_drawText( "Background PRIORITY: OFF                ", 0, 4 );
    VDP_drawText( "A:SH on/off, B:Palette, C:Circle Prio  ", 0, 26 );
    VDP_drawText( "X: color14/15, Y: Background Priority  ", 0, 27 );

    // shadow/highlight starts disabled
    VDP_setHilightShadow( 0 );

    while ( TRUE ) {

        // read joypad (synchronous) and move the circle
        handleInput();

        // flush sprite updates to the VDP
        SPR_update();

        // sync with the TV vertical blank
        SYS_doVBlankProcess();

    }

    return 0;

}

// synchronous input — moves the circle sprite with the d-pad
static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        SPR_setPosition( targetSprite, targetPosx--, targetPosy );
    }

    if ( value & BUTTON_RIGHT ) {
        SPR_setPosition( targetSprite, targetPosx++, targetPosy );
    }

    if ( value & BUTTON_UP ) {
        SPR_setPosition( targetSprite, targetPosx, targetPosy-- );
    }

    if ( value & BUTTON_DOWN ) {
        SPR_setPosition( targetSprite, targetPosx, targetPosy++ );
    }

}

// asynchronous input — A/B/C/X/Y toggle the various shadow/highlight parameters
void myJoyHandler( u16 joy, u16 changed, u16 state ) {

    if ( joy == JOY_1 ) {

        // A — toggle shadow/highlight on/off
        if ( state & BUTTON_A ) {
            if ( shActive ) {
                shActive = 0;
            } else {
                shActive = 1;
            }
            VDP_setHilightShadow( shActive );
        }

        // B — toggle circle palette between PAL2 and PAL3
        if ( state & BUTTON_B ) {
            if ( shPalette == 3 ) {
                shPalette = 2;
                SPR_setPalette( targetSprite, PAL2 );
            } else {
                shPalette = 3;
                SPR_setPalette( targetSprite, PAL3 );
            }
        }

        // C — toggle circle sprite priority
        if ( state & BUTTON_C ) {
            if ( shPriority ) {
                shPriority = 0;
                SPR_setPriority( targetSprite, FALSE );
            } else {
                shPriority = 1;
                SPR_setPriority( targetSprite, TRUE );
            }
        }

        // X — toggle circle between color index 14 and 15
        if ( state & BUTTON_X ) {
            if ( shColor == 14 ) {
                shColor = 15;
                SPR_setAnim( targetSprite, CIRCLE15 );
            } else {
                shColor = 14;
                SPR_setAnim( targetSprite, CIRCLE14 );
            }
        }

        // Y — toggle background priority (redraw background with/without high priority)
        if ( state & BUTTON_Y ) {
            if ( shBgPriority ) {
                shBgPriority = 0;
                ind = TILE_USER_INDEX;
                VDP_drawImageEx( BG_A, &fondo1, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
                ind += fondo1.tileset->numTile;
            } else {
                shBgPriority = 1;
                ind = TILE_USER_INDEX;
                VDP_drawImageEx( BG_A, &fondo1, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
                ind += fondo1.tileset->numTile;
            }
        }

    }

    drawHelp();

}

// NOTE: the conditions below are intentionally inverted in the original source —
// the display labels are swapped relative to the actual state values.
void drawHelp( void ) {

    if ( shActive ) {
        VDP_drawText( "S&H: OFF                                ", 0, 0 );
    } else {
        VDP_drawText( "S&H: ON                                 ", 0, 0 );
    }

    if ( shPalette == 3 ) {
        VDP_drawText( "CIRCLE PALETTE: PAL2                    ", 0, 1 );
    } else {
        VDP_drawText( "CIRCLE PALETTE: PAL3                    ", 0, 1 );
    }

    if ( shPriority ) {
        VDP_drawText( "Priority: NO                            ", 0, 2 );
    } else {
        VDP_drawText( "Priority: YES                           ", 0, 2 );
    }

    if ( shColor == 14 ) {
        VDP_drawText( "Circle color: 15                        ", 0, 3 );
    } else {
        VDP_drawText( "Circle color: 14                        ", 0, 3 );
    }

    if ( shBgPriority ) {
        VDP_drawText( "Background PRIORITY: OFF                ", 0, 4 );
    } else {
        VDP_drawText( "Background PRIORITY: ON                 ", 0, 4 );
    }

    VDP_drawText( "A:SH on/off, B:Palette, C:Circle Prio  ", 0, 26 );
    VDP_drawText( "X: color14/15, Y: Background Priority  ", 0, 27 );

}
