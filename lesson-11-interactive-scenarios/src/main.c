/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 11 - "Interactive Scenarios" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

/*
    EXAMPLE 0: palette tint using red palette (PAL3)
    EXAMPLE 1: slow palette tint using red palette (PAL3)
    EXAMPLE 2: single-color tint (color index 9 of PAL0)
    EXAMPLE 3: tint using an existing palette (PAL1)
    EXAMPLE 4: full plane A clear
    EXAMPLE 5: partial plane A clear
    EXAMPLE 6: destructible towers — tile repaint
*/

#include <genesis.h>
#include "sprite.h"
#include "fondos.h"
#include <sys.h>

#define OFF_SCREEN        -100  // position used to hide sprites outside the visible area
#define MAX_TOWERS          2
#define TINT_DURATION       2   // cycles the tower stays red after a hit
#define TINT_DURATION_SLOW  9   // longer tint duration (example 1)
#define AIM_OFFSET          8   // shot spawn offset relative to the aim crosshair

// function declarations
void joyEvent( u16 joy, u16 changed, u16 state );

void createLevel( void );
void createScenery( void );
void updateScenery( void );
void createAim( void );
void updateAim( void );

void createPlayerShot( void );
void firePlayerShot( void );
void updatePlayerShot( void );

void detectCollision( void );

void drawMessages( void );
void drawHealth( void );

// demo state
int locked  = 1; // inner loop lock; set to 0 to advance to the next example
int example = 0;

// aim crosshair
struct {
    s16     x, y;
    s16     movx, movy;
    Sprite *sprite;
} aim;

// player shot
struct {
    Sprite *sprite;
    s16     x, y;
    int     active; // 0=inactive; 1–4=active, corresponds to the 4 shot animation frames
} playerShot;

// tower
struct {
    int health;
    int state;        // 0=intact, 1=half-destroyed, 2=ruins
    s16 colX1, colY1; // hitbox top-left
    s16 colX2, colY2; // hitbox bottom-right
    s16 hitActive;
    s16 hitTimer;
} tower[MAX_TOWERS];

TileMap *towerMap;

u16 tileStart;       // always 1 (tile 0 is reserved by SGDK)
u16 tileFinalBgB;    // first free VRAM slot after BG_B tiles
u16 tileFinalBgA;    // first free VRAM slot after BG_A tiles


int main( bool hard ) {

    VDP_setScreenWidth320();
    VDP_setScreenHeight224();

    PAL_setPalette( PAL0, fondo01_01.palette->data, CPU );
    PAL_setPalette( PAL1, sprite_mira.palette->data, CPU );
    // PAL2 is left at the SGDK default (greens)
    PAL_setPalette( PAL3, palette_red, CPU );

    SPR_init();

    createLevel();
    createPlayerShot(); // created before the aim so it renders behind it
    createAim();

    JOY_setEventHandler( joyEvent );

    while ( TRUE ) {

        createScenery();
        drawMessages();

        while ( locked ) {

            updateScenery();
            updateAim();
            updatePlayerShot();
            drawHealth();

            SPR_update();
            SYS_doVBlankProcess();

        }

        example++;

    }

    return 0;

}


void createLevel( void ) {

    SYS_disableInts();

    VDP_clearPlane( BG_A, TRUE );
    VDP_clearPlane( BG_B, TRUE );

    tileStart = 1;

    VDP_drawImageEx( BG_B, &fondo01_01, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileStart ), 0, 0, FALSE, TRUE );
    tileFinalBgB += fondo01_01.tileset->numTile;

    towerMap = unpackTileMap( fondo01_01_torres.tilemap, NULL );
    VDP_loadTileSet( fondo01_01_torres.tileset, tileFinalBgB, CPU );
    VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 0, 0, 10, 11 );
    VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), 31, 4, 0, 0, 10, 11 );
    tileFinalBgA += fondo01_01.tileset->numTile;

    SYS_enableInts();

}


void createAim( void ) {

    aim.x    = 100;
    aim.y    = 100;
    aim.movx = 2;
    aim.movy = 2;

    aim.sprite = SPR_addSprite( &sprite_mira, aim.x, aim.y, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ) );

}


void updateAim( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        SPR_setPosition( aim.sprite, aim.x -= aim.movx, aim.y );
        aim.x -= aim.movx;
    }

    if ( value & BUTTON_RIGHT ) {
        SPR_setPosition( aim.sprite, aim.x += aim.movx, aim.y );
        aim.x += aim.movx;
    }

    if ( value & BUTTON_UP ) {
        SPR_setPosition( aim.sprite, aim.x, aim.y -= aim.movy );
        aim.y -= aim.movy;
    }

    if ( value & BUTTON_DOWN ) {
        SPR_setPosition( aim.sprite, aim.x, aim.y += aim.movy );
        aim.y += aim.movy;
    }

    if ( value & BUTTON_A ) { firePlayerShot(); }

}


// async button-release detection used to advance between examples
void joyEvent( u16 joy, u16 changed, u16 state ) {

    if ( state & BUTTON_B ) {
        // button B held — wait for release
    } else if ( changed & BUTTON_B ) {
        locked = 0;
    }

}


void createScenery( void ) {

    tower[0].health    = 80;
    tower[0].state     = 0;
    tower[0].colX1     =   5;
    tower[0].colY1     =  40;
    tower[0].colX2     =  60;
    tower[0].colY2     = 110;
    tower[0].hitActive = 0;
    tower[0].hitTimer  = 0;

    tower[1].health   = 80;
    tower[1].state    = 0;
    tower[1].colX1    = 255;
    tower[1].colY1    =  40;
    tower[1].colX2    = 320;
    tower[1].colY2    = 110;
    tower[1].hitTimer = 0;

    locked = 1;

}


void updateScenery( void ) {

    // EXAMPLE 0: red palette tint
    if ( example == 0 ) {

        if ( tower[0].hitActive != 0 ) {

            tower[0].hitTimer++;

            if ( tower[0].hitTimer == 1 ) {
                VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL3, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 0, 0, 10, 11 );
            } else if ( tower[0].hitTimer >= TINT_DURATION ) {
                tower[0].hitActive = 0;
                tower[0].hitTimer  = 0;
                VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 0, 0, 10, 11 );
            }

        }

    }

    // EXAMPLE 1: slow red palette tint
    if ( example == 1 ) {

        if ( tower[0].hitActive != 0 ) {

            tower[0].hitTimer++;

            if ( tower[0].hitTimer == 1 ) {
                VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL3, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 0, 0, 10, 11 );
            } else if ( tower[0].hitTimer >= TINT_DURATION_SLOW ) {
                tower[0].hitActive = 0;
                tower[0].hitTimer  = 0;
                VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 0, 0, 10, 11 );
            }

        }

    }

    // EXAMPLE 2: single-color tint (color index 9 of PAL0)
    if ( example == 2 ) {

        if ( tower[0].hitActive != 0 ) {

            tower[0].hitTimer++;

            if ( tower[0].hitTimer == 1 ) {
                PAL_setColor( 9, RGB24_TO_VDPCOLOR( 0xff0000 ) );
            } else if ( tower[0].hitTimer >= TINT_DURATION ) {
                PAL_setColor( 9, RGB24_TO_VDPCOLOR( 0x4c260c ) );
                tower[0].hitActive = 0;
                tower[0].hitTimer  = 0;
            }

        }

    }

    // EXAMPLE 3: tint using an existing palette (PAL1)
    if ( example == 3 ) {

        if ( tower[0].hitActive != 0 ) {

            tower[0].hitTimer++;

            if ( tower[0].hitTimer == 1 ) {
                VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL1, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 0, 0, 10, 11 );
            } else if ( tower[0].hitTimer >= TINT_DURATION ) {
                tower[0].hitActive = 0;
                tower[0].hitTimer  = 0;
                VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 0, 0, 10, 11 );
            }

        }

    }

    // EXAMPLE 4: full plane A clear
    if ( example == 4 ) {

        if ( tower[0].hitActive != 0 ) {

            tower[0].hitTimer++;

            if ( tower[0].hitTimer == 1 ) {
                VDP_clearPlane( BG_A, TRUE );
                VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), 31, 4, 0, 0, 10, 11 );
                drawMessages();
            } else if ( tower[0].hitTimer >= TINT_DURATION ) {
                tower[0].hitActive = 0;
                tower[0].hitTimer  = 0;
                VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 0, 0, 10, 11 );
            }

        }

    }

    // EXAMPLE 5: partial plane A clear (only the left tower above the wall)
    if ( example == 5 ) {

        if ( tower[0].hitActive != 0 ) {

            tower[0].hitTimer++;

            if ( tower[0].hitTimer == 1 ) {
                VDP_clearTileMapRect( BG_A, 0, 3, 10, 5 );
            } else if ( tower[0].hitTimer >= TINT_DURATION ) {
                tower[0].hitActive = 0;
                tower[0].hitTimer  = 0;
                VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 0, 0, 10, 5 );
            }

        }

    }

    // EXAMPLE 6: destructible towers with tile repaint
    if ( example == 6 ) {

        if ( tower[0].health < 40 && tower[0].state == 0 ) {
            tower[0].state = 1;
            towerMap = unpackTileMap( fondo01_01_torres.tilemap, NULL );
            VDP_loadTileSet( fondo01_01_torres.tileset, tileFinalBgB, CPU );
            VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 10, 0, 10, 11 );
            tileFinalBgA += fondo01_01.tileset->numTile;
        }

        if ( tower[0].health < 20 && tower[0].state == 1 ) {
            tower[0].state = 2;
            VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), -1, 3, 20, 0, 10, 11 );
            tileFinalBgA += fondo01_01.tileset->numTile;
        }

        if ( tower[1].health < 40 && tower[1].state == 0 ) {
            tower[1].state = 1;
            towerMap = unpackTileMap( fondo01_01_torres.tilemap, NULL );
            VDP_loadTileSet( fondo01_01_torres.tileset, tileFinalBgB, CPU );
            VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), 31, 4, 10, 0, 10, 11 );
            tileFinalBgA += fondo01_01.tileset->numTile;
        }

        if ( tower[1].health < 20 && tower[1].state == 1 ) {
            tower[1].state = 2;
            VDP_setMapEx( BG_A, towerMap, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, tileFinalBgB ), 31, 4, 20, 0, 10, 11 );
        }

    }

    // RESET
    if ( example == 7 ) {
        SYS_hardReset();
    }

}


void createPlayerShot( void ) {

    playerShot.active = 0;
    playerShot.x      = OFF_SCREEN;
    playerShot.y      = OFF_SCREEN;
    playerShot.sprite = SPR_addSprite(
        &sprite_bala_player,
        playerShot.x,
        playerShot.y,
        TILE_ATTR( PAL1, TRUE, FALSE, FALSE )
    );

    SPR_setDepth( playerShot.sprite, SPR_MIN_DEPTH );

}


// activates and positions the shot at the aim center (blocked while already active)
void firePlayerShot( void ) {

    if ( playerShot.active == 0 ) {
        playerShot.active = 1;
        playerShot.x      = aim.x + AIM_OFFSET;
        playerShot.y      = aim.y + AIM_OFFSET;
        SPR_setPosition( playerShot.sprite, playerShot.x, playerShot.y );
    }

}


// advances animation frames and hides the shot when the 4-frame animation completes
void updatePlayerShot( void ) {

    if ( playerShot.active >= 1 && playerShot.active <= 4 ) {
        SPR_nextFrame( playerShot.sprite );
        playerShot.active++;
        detectCollision();
    }

    if ( playerShot.active > 4 ) {
        playerShot.x      = OFF_SCREEN;
        playerShot.y      = OFF_SCREEN;
        SPR_setPosition( playerShot.sprite, playerShot.x, playerShot.y );
        playerShot.active = 0;
    }

}


// runs only during the 4 active frames of the shot animation for efficiency
void detectCollision( void ) {

    for ( s16 i = 0; i < MAX_TOWERS; i++ ) {

        if ( tower[i].health > 0 ) {

            if ( playerShot.x > tower[i].colX1 && playerShot.x < tower[i].colX2 &&
                 playerShot.y > tower[i].colY1 && playerShot.y < tower[i].colY2 ) {
                tower[i].health--;
                tower[i].hitActive++;
            }

        }

    }

}


void drawMessages( void ) {

    VDP_setTextPalette( PAL1 );

    if ( example == 0 ) {
        VDP_drawText( "EXAMPLE 0: PALETTE TINT (PAL3 RED)      ", 0, 20 );
        VDP_drawText( "     Switches between PAL0 and PAL3     ", 0, 21 );
        VDP_drawText( "  (fire on the left tower)              ", 0, 22 );
        VDP_drawText( "     Press -A- to fire                  ", 0, 23 );
        VDP_drawText( "     Press -B- to continue              ", 0, 24 );
    }

    if ( example == 1 ) {
        VDP_drawText( "EXAMPLE 1: SLOW PALETTE TINT (PAL3 RED) ", 0, 20 );
        VDP_drawText( "  Switches PAL0 / PAL3 with longer      ", 0, 21 );
        VDP_drawText( "  duration before restoring normal pal  ", 0, 22 );
        VDP_drawText( "                                        ", 0, 23 );
        VDP_drawText( "     Press -B- to continue              ", 0, 24 );
    }

    if ( example == 2 ) {
        VDP_drawText( "EXAMPLE 2: SINGLE COLOR TINT            ", 0, 20 );
        VDP_drawText( "                                        ", 0, 21 );
        VDP_drawText( "  Changes color index 9 of the palette  ", 0, 22 );
        VDP_drawText( "                                        ", 0, 23 );
        VDP_drawText( "     Press -B- to continue              ", 0, 24 );
    }

    if ( example == 3 ) {
        VDP_drawText( "EXAMPLE 3: TINT USING EXISTING PALETTE  ", 0, 20 );
        VDP_drawText( "                                        ", 0, 21 );
        VDP_drawText( "  Switches between PAL0 and PAL1        ", 0, 22 );
        VDP_drawText( "                                        ", 0, 23 );
        VDP_drawText( "     Press -B- to continue              ", 0, 24 );
    }

    if ( example == 4 ) {
        VDP_drawText( "EXAMPLE 4: FULL PLANE A CLEAR           ", 0, 20 );
        VDP_drawText( "  Clears plane A entirely, then redraws ", 0, 21 );
        VDP_drawText( "  the right tower.                      ", 0, 22 );
        VDP_drawText( "                                        ", 0, 23 );
        VDP_drawText( "     Press -B- to continue              ", 0, 24 );
    }

    if ( example == 5 ) {
        VDP_drawText( "EXAMPLE 5: PARTIAL PLANE A CLEAR        ", 0, 20 );
        VDP_drawText( "  Clears only the left-tower portion    ", 0, 21 );
        VDP_drawText( "  of plane A (above the wall).          ", 0, 22 );
        VDP_drawText( "                                        ", 0, 23 );
        VDP_drawText( "     Press -B- to continue              ", 0, 24 );
    }

    if ( example == 6 ) {
        VDP_drawText( "EXAMPLE 6: DESTRUCTIBLE TOWERS          ", 0, 20 );
        VDP_drawText( "                                        ", 0, 21 );
        VDP_drawText( "  Fire on both towers to destroy them.  ", 0, 22 );
        VDP_drawText( "  Tile repaint technique.               ", 0, 23 );
        VDP_drawText( "     Press -B- to RESET                 ", 0, 24 );
    }

}


void drawHealth( void ) {

    char buf1[32], buf2[32];

    sprintf( buf1, "%4d", tower[0].health );
    VDP_drawText( "Tower[0].health:", 1, 15 );
    VDP_drawText( buf1, 1, 16 );

    if ( example == 6 ) {
        sprintf( buf2, "%4d", tower[1].health );
        VDP_drawText( "Tower[1].health:", 25, 15 );
        VDP_drawText( buf2, 32, 16 );
    }

}
