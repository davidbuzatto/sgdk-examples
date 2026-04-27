/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 09 - "Scroll (0) - By Planes" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

/*
    STATE 1: initial state — plane A horizontal scroll
    STATE 2: plane A horizontal + vertical scroll
    STATE 3: plane B horiz + vert scroll, plane A fixed
    STATE 4: planes A and B scroll in all directions
    STATE 5: planes A and B horizontal scroll only — sprite fixed
    STATE 6: planes A, B and sprite move (sprite clamped to visible area)
*/

#include <genesis.h>

#include "gfx.h"
#include "sprite.h"
#include <sys.h>

// Sonic animations
#define ANIM_STAND  0
#define ANIM_RUN    3

// sprite screen limits
#define LIMIT_LEFT   20
#define LIMIT_RIGHT 260

// function declarations
static void handleInput( void );
static void loadState( void );
static void drawMessages( void );
static void updateCamera( void );

// Sonic sprite pointer
Sprite *sonicSprite;

// demo state — increments each time the user advances to the next example
int state  = 1;
int locked = 0; // inner loop runs while locked==1; set to 0 to advance state

// tile counter in VRAM
u16 ind;

// scroll offsets relative to (0, 0) of each plane
s16 offsetHPlaneA = 0;
s16 offsetVPlaneA = 0;
s16 offsetHPlaneB = 0;
s16 offsetVPlaneB = 0;

// scroll speed per state
int speedPlaneA = 2;
int speedPlaneB = 1;

// Sonic position
int speedSonic = 3;
s16 sonicPosx = 48;
s16 sonicPosy = 158;

int main( bool hard ) {

    VDP_setScreenWidth320();

    // load palettes
    PAL_setPalette( PAL0, bgb_image.palette->data, CPU );
    PAL_setPalette( PAL1, bga_image.palette->data, CPU );
    PAL_setPalette( PAL2, sonic_sprite.palette->data, CPU );

    SPR_init();

    // load both backgrounds
    ind = TILE_USER_INDEX;
    VDP_drawImageEx( BG_B, &bgb_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bgb_image.tileset->numTile;
    VDP_drawImageEx( BG_A, &bga_image, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bga_image.tileset->numTile;

    // configure plane-level horizontal and vertical scroll
    VDP_setScrollingMode( HSCROLL_PLANE, VSCROLL_PLANE );

    while ( TRUE ) {

        // initialise the current example — called once each time state changes
        loadState();

        // inner loop runs for the duration of each example
        while ( locked ) {

            handleInput();
            drawMessages();
            updateCamera();

            SPR_update();
            SYS_doVBlankProcess();

        }

        // advance to the next example
        state++;

    }

    return 0;

}

// reset scroll offsets and set up any state-specific resources
static void loadState( void ) {

    offsetHPlaneA = 0; offsetVPlaneA = 0;
    offsetHPlaneB = 0; offsetVPlaneB = 0;

    // state 5 adds a Sonic sprite
    if ( state == 5 ) {
        sonicSprite = SPR_addSprite( &sonic_sprite, sonicPosx, sonicPosy, TILE_ATTR( PAL2, TRUE, FALSE, FALSE ) );
    }

    locked = 1; // lock the inner loop

}

// applies scroll offsets and sprite position for the current state
static void updateCamera( void ) {

    if ( state == 1 ) {
        VDP_setHorizontalScroll( BG_A, offsetHPlaneA );
    }

    if ( state == 2 ) {
        VDP_setHorizontalScroll( BG_A, offsetHPlaneA );
        VDP_setVerticalScroll(   BG_A, offsetVPlaneA );
    }

    if ( state == 3 ) {
        // plane A is fixed — explicitly set scroll to zero so it renders from the origin
        VDP_setHorizontalScroll( BG_A, 0 );
        VDP_setVerticalScroll(   BG_A, 0 );
        VDP_setHorizontalScroll( BG_B, offsetHPlaneB );
        VDP_setVerticalScroll(   BG_B, offsetVPlaneB );
    }

    if ( state == 4 ) {
        VDP_setHorizontalScroll( BG_A, offsetHPlaneA );
        VDP_setVerticalScroll(   BG_A, offsetVPlaneA );
        VDP_setHorizontalScroll( BG_B, offsetHPlaneB );
        VDP_setVerticalScroll(   BG_B, offsetVPlaneB );
    }

    if ( state == 5 ) {
        VDP_setHorizontalScroll( BG_A, offsetHPlaneA );
        VDP_setVerticalScroll(   BG_A, offsetVPlaneA );
        VDP_setHorizontalScroll( BG_B, offsetHPlaneB );
        VDP_setVerticalScroll(   BG_B, offsetVPlaneB );
    }

    if ( state == 6 ) {
        VDP_setHorizontalScroll( BG_A, offsetHPlaneA );
        VDP_setVerticalScroll(   BG_A, offsetVPlaneA );
        VDP_setHorizontalScroll( BG_B, offsetHPlaneB );
        VDP_setVerticalScroll(   BG_B, offsetVPlaneB );
        SPR_setPosition( sonicSprite, sonicPosx, sonicPosy );
    }

}

static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( state == 1 ) { // d-pad: move plane A (left/right only) | A: next
        if ( value & BUTTON_LEFT ) {
            offsetHPlaneA += speedPlaneA;
        } else if ( value & BUTTON_RIGHT ) {
            offsetHPlaneA -= speedPlaneA;
        }
        if ( value & BUTTON_A ) { locked = 0; }
    }

    if ( state == 2 ) { // d-pad: move plane A (all directions) | B: next
        if ( value & BUTTON_LEFT ) {
            offsetHPlaneA += speedPlaneA;
        } else if ( value & BUTTON_RIGHT ) {
            offsetHPlaneA -= speedPlaneA;
        }
        if ( value & BUTTON_UP ) {
            offsetVPlaneA += speedPlaneA;
        } else if ( value & BUTTON_DOWN ) {
            offsetVPlaneA -= speedPlaneA;
        }
        if ( value & BUTTON_B ) { locked = 0; }
    }

    if ( state == 3 ) { // d-pad: move plane B | C: next
        if ( value & BUTTON_LEFT ) {
            offsetHPlaneB -= speedPlaneB;
        } else if ( value & BUTTON_RIGHT ) {
            offsetHPlaneB += speedPlaneB;
        }
        if ( value & BUTTON_UP ) {
            offsetVPlaneB += speedPlaneB;
        } else if ( value & BUTTON_DOWN ) {
            offsetVPlaneB -= speedPlaneB;
        }
        if ( value & BUTTON_C ) { locked = 0; }
    }

    if ( state == 4 ) { // d-pad: move both planes | A: next
        if ( value & BUTTON_LEFT ) {
            offsetHPlaneA += speedPlaneA;
        } else if ( value & BUTTON_RIGHT ) {
            offsetHPlaneA -= speedPlaneA;
        }
        if ( value & BUTTON_UP ) {
            offsetVPlaneA += speedPlaneA;
        } else if ( value & BUTTON_DOWN ) {
            offsetVPlaneA -= speedPlaneA;
        }
        if ( value & BUTTON_LEFT ) {
            offsetHPlaneB += speedPlaneB;
        } else if ( value & BUTTON_RIGHT ) {
            offsetHPlaneB -= speedPlaneB;
        }
        if ( value & BUTTON_UP ) {
            offsetVPlaneB += speedPlaneB;
        } else if ( value & BUTTON_DOWN ) {
            offsetVPlaneB -= speedPlaneB;
        }
        if ( value & BUTTON_A ) { locked = 0; }
    }

    if ( state == 5 ) { // d-pad: scroll planes and animate sprite | B: next
        if ( value & BUTTON_LEFT ) {
            offsetHPlaneA += speedPlaneA;
            offsetHPlaneB += speedPlaneB;
            SPR_setAnim( sonicSprite, ANIM_RUN );
        }
        if ( value & BUTTON_RIGHT ) {
            offsetHPlaneA -= speedPlaneA;
            offsetHPlaneB -= speedPlaneB;
            SPR_setAnim( sonicSprite, ANIM_RUN );
        }
        if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) ) {
            SPR_setAnim( sonicSprite, ANIM_STAND );
        }
        if ( value & BUTTON_B ) { locked = 0; }
    }

    if ( state == 6 ) { // d-pad: scroll planes and move sprite with limits | C: hard reset
        if ( value & BUTTON_LEFT ) {
            offsetHPlaneA += speedPlaneA;
            offsetHPlaneB += speedPlaneB;
            sonicPosx--;
            if ( sonicPosx <= LIMIT_LEFT ) { sonicPosx = LIMIT_LEFT; }
            SPR_setAnim( sonicSprite, ANIM_RUN );
            SPR_setHFlip( sonicSprite, TRUE );
        }
        if ( value & BUTTON_RIGHT ) {
            sonicPosx++;
            if ( sonicPosx >= LIMIT_RIGHT ) { sonicPosx = LIMIT_RIGHT; }
            SPR_setAnim( sonicSprite, ANIM_RUN );
            SPR_setHFlip( sonicSprite, FALSE );
            offsetHPlaneA -= speedPlaneA;
            offsetHPlaneB -= speedPlaneB;
        }
        if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) ) {
            SPR_setAnim( sonicSprite, ANIM_STAND );
        }
        if ( value & BUTTON_C ) {
            SYS_hardReset();
        }
    }

}

static void drawMessages( void ) {

    if ( state == 1 ) {
        VDP_drawText( "PLANE A  H-scroll: ON,  V-scroll: OFF  ", 0, 0 );
        VDP_drawText( "PLANE B  off                           ", 0, 1 );
        VDP_drawText( "PLANE W  off                           ", 0, 2 );
        VDP_drawText( "D-PAD: move plane A (left/right only)  ", 0, 3 );
        VDP_drawText( "Press -A- to continue                  ", 0, 4 );
    }

    if ( state == 2 ) {
        VDP_drawText( "PLANE A  H-scroll: ON,  V-scroll: ON   ", 0, 0 );
        VDP_drawText( "PLANE B  off                           ", 0, 1 );
        VDP_drawText( "PLANE W  off                           ", 0, 2 );
        VDP_drawText( "D-PAD: move plane A (all directions)   ", 0, 3 );
        VDP_drawText( "Press -B- to continue                  ", 0, 4 );
    }

    if ( state == 3 ) {
        VDP_drawText( "PLANE A  off                           ", 0, 0 );
        VDP_drawText( "PLANE B  H-scroll: ON,  V-scroll: ON   ", 0, 1 );
        VDP_drawText( "PLANE W  off                           ", 0, 2 );
        VDP_drawText( "D-PAD: move plane B                    ", 0, 3 );
        VDP_drawText( "Press -C- to continue                  ", 0, 4 );
    }

    if ( state == 4 ) {
        VDP_drawText( "PLANE A  H-scroll: ON,  V-scroll: ON   ", 0, 0 );
        VDP_drawText( "PLANE B  H-scroll: ON,  V-scroll: ON   ", 0, 1 );
        VDP_drawText( "PLANE W  off                           ", 0, 2 );
        VDP_drawText( "D-PAD: move planes A and B             ", 0, 3 );
        VDP_drawText( "Press -A- to continue                  ", 0, 4 );
    }

    if ( state == 5 ) {
        VDP_drawText( "PLANE A  H-scroll: ON,  V-scroll: OFF  ", 0, 0 );
        VDP_drawText( "PLANE B  H-scroll: ON,  V-scroll: OFF  ", 0, 1 );
        VDP_drawText( "PLANE W  off                           ", 0, 2 );
        VDP_drawText( "D-PAD: scroll planes. SPRITE IS FIXED! ", 0, 3 );
        VDP_drawText( "Press -B- to continue                  ", 0, 4 );
    }

    if ( state == 6 ) {
        VDP_drawText( "PLANE A  H-scroll: ON,  V-scroll: OFF  ", 0, 0 );
        VDP_drawText( "PLANE B  H-scroll: ON,  V-scroll: OFF  ", 0, 1 );
        VDP_drawText( "PLANE W  off                           ", 0, 2 );
        VDP_drawText( "SPRITE moves across the screen         ", 0, 3 );
        VDP_drawText( "Press -C- to reset                     ", 0, 4 );
    }

}
