/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 09 - "Scroll (1) - By Tiles" example for SGDK.
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
    STATE 6: planes A, B and sprite (sprite clamped to visible area)
    STATE 7: shoot-em-up demo
    STATE 8: vertical scroll demo
*/

#include <genesis.h>

#include "gfx.h"
#include "sprite.h"
#include <sys.h>

// Sonic animations
#define ANIM_STAND      0
#define ANIM_RUN        3

// ship animations
#define ANIM_SHIP_IDLE  0
#define ANIM_SHIP_MOVE  1

// sprite screen limits
#define LIMIT_LEFT    10
#define LIMIT_RIGHT  280
#define LIMIT_UP      10
#define LIMIT_DOWN   180

// function declarations
static void handleInput( void );
static void loadState( void );
static void drawMessages( void );
static void updateCamera( void );

// sprite pointers
Sprite *sonicSprite;
Sprite *shipSprite;
Sprite *enemies[6];

// demo state
int state  = 1;
int locked = 0;

// tile counter in VRAM
u16 ind;

// scroll offset for state 6 (plane A horizontal)
s16 offsetA    = 0;
s16 speedA     = 4;

// Sonic position (state 6)
int sonicPosx = 48;
int sonicPosy = 158;

// ship and enemy positions (state 7)
int enemyPosx[6] = { 0, 0, 0, 0, 0, 0 };
int enemyPosy[6] = { 0, 0, 0, 0, 0, 0 };

// per-tile scroll vectors
s16 vector01[6]  = { 1, 2, 3, 4, 5, 6 };
s16 vector02[6]  = { 0, 10, 20, 30, 40, 50 };
s16 vector03[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
s16 vector04[28] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
s16 vector05[28] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
s16 vector06[28] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

s16 vector07[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// per-tile acceleration vectors
s16 vAccel01[6]  = { 1, 2, 3, 3, 2, 1 };
s16 vAccel02[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
s16 vAccel03[28] = { 9, 9, 8, 8, 7, 6, 6, 6, 5, 5, 4, 3, 2, 1, 2, 3, 4, 5, 5, 6 };
s16 vAccel04[20] = { 10, 9, 8, 5, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

fix16 vectorAux[28] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
fix16 accel04 = FIX16( 0.06 );

int main( bool hard ) {

    VDP_setScreenWidth320();

    PAL_setPalette( PAL0, bgb_image.palette->data, CPU );
    PAL_setPalette( PAL1, bga_image.palette->data, CPU );
    PAL_setPalette( PAL2, sonic_sprite.palette->data, CPU );

    SPR_init();

    ind = TILE_USER_INDEX;
    VDP_drawImageEx( BG_B, &bgb_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bgb_image.tileset->numTile;
    VDP_drawImageEx( BG_A, &bga_image, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bga_image.tileset->numTile;

    // configure horizontal tile scroll (vertical stays at plane-level default)
    VDP_setScrollingMode( HSCROLL_TILE, VSCROLL_PLANE );

    while ( TRUE ) {

        loadState();

        while ( locked ) {

            handleInput();
            drawMessages();
            updateCamera();

            SPR_update();
            SYS_doVBlankProcess();

        }

        state++;

    }

    return 0;

}

// initialise the current state's scroll vectors and load required resources
static void loadState( void ) {

    offsetA = 0;

    // reset vectors that change during the examples
    for ( int i = 0; i < 6;  i++ ) { vector01[i] = i + 1; }   // 1..6
    for ( int i = 0; i < 6;  i++ ) { vector02[i] = i * 10; }  // 0..50
    for ( int i = 0; i < 6;  i++ ) { vector03[i] = 0; }
    for ( int i = 0; i < 28; i++ ) { vector04[i] = 0; }
    for ( int i = 0; i < 28; i++ ) { vector05[i] = FIX16( 0 ); }

    if ( state == 6 ) {
        sonicSprite = SPR_addSprite( &sonic_sprite, sonicPosx, sonicPosy, TILE_ATTR( PAL2, TRUE, FALSE, FALSE ) );
    }

    if ( state == 7 ) {
        VDP_resetScreen();
        VDP_init();
        VDP_setScreenWidth320();
        SPR_init();

        PAL_setPalette( PAL0, bgd_image.palette->data, CPU );
        PAL_setPalette( PAL0, bgc_image.palette->data, CPU );
        PAL_setPalette( PAL0, ship_sprite.palette->data, CPU );

        SPR_init();

        ind = TILE_USER_INDEX;
        VDP_drawImageEx( BG_B, &bgd_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
        ind += bgd_image.tileset->numTile;
        VDP_drawImageEx( BG_A, &bgc_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
        ind += bgc_image.tileset->numTile;

        VDP_setScrollingMode( HSCROLL_TILE, VSCROLL_PLANE );

        // initial sprite positions
        sonicPosx = 50;
        sonicPosy = 120;

        for ( int i = 0; i < 6; i++ ) {
            enemyPosx[i] = 100 + i * 50;
            enemyPosy[i] = 30  + i * 20;
        }

        shipSprite = SPR_addSprite( &ship_sprite, sonicPosx, sonicPosy, TILE_ATTR( PAL0, TRUE, FALSE, FALSE ) );

        for ( int i = 0; i < 6; i++ ) {
            enemies[i] = SPR_addSprite( &enemy_sprite, enemyPosx[i], enemyPosy[i], TILE_ATTR( PAL0, TRUE, FALSE, FALSE ) );
        }
    }

    if ( state == 8 ) {
        VDP_resetScreen();
        VDP_init();
        VDP_setScreenWidth320();
        SPR_init();

        PAL_setPalette( PAL0, bgv_image.palette->data, CPU );

        ind = TILE_USER_INDEX;
        VDP_drawImageEx( BG_A, &bgv_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
        ind += bgv_image.tileset->numTile;

        // configure vertical tile scroll
        VDP_setScrollingMode( HSCROLL_PLANE, VSCROLL_COLUMN );
    }

    locked = 1;

}

// scrolls the appropriate planes and tiles for the current state
static void updateCamera( void ) {

    if ( state == 1 ) { // plane A fixed; scroll first 6 tile rows of plane B
        VDP_setHorizontalScrollTile( BG_B, 0, vector01, 6, CPU );
        for ( int i = 0; i < 6; i++ ) { vector01[i] += 1; }
    }

    if ( state == 2 ) { // same as state 1 but starting with a different offset vector
        VDP_setHorizontalScrollTile( BG_B, 0, vector02, 6, CPU );
        for ( int i = 0; i < 6; i++ ) { vector02[i] += 1; }
    }

    if ( state == 3 ) { // different speed per tile row
        VDP_setHorizontalScrollTile( BG_B, 0, vector02, 6, CPU );
        for ( int i = 0; i < 6; i++ ) { vector02[i] += i; }
    }

    if ( state == 4 ) { // per-row acceleration via vAccel01
        VDP_setHorizontalScrollTile( BG_B, 0, vector02, 6, CPU );
        for ( int i = 0; i < 6; i++ ) { vector02[i] += vAccel01[i]; }
    }

    if ( state == 5 ) { // two independent zones: clouds (rows 0-5) and sea (rows 18-27)
        VDP_setHorizontalScrollTile( BG_B, 0,  vector01, 6,  CPU );
        for ( int i = 0; i < 6;  i++ ) { vector01[i] += 2; }

        VDP_setHorizontalScrollTile( BG_B, 18, vector03, 10, CPU );
        for ( int i = 0; i < 10; i++ ) { vector03[i] -= vAccel02[i]; }
    }

    if ( state == 6 ) { // plane B: two zones; plane A: full-plane offset
        // clouds
        VDP_setHorizontalScrollTile( BG_B, 0,  vector01, 6,  CPU );
        for ( int i = 0; i < 6;  i++ ) { vector01[i] -= 2; }
        // sea
        VDP_setHorizontalScrollTile( BG_B, 18, vector03, 10, CPU );
        for ( int i = 0; i < 10; i++ ) { vector03[i] -= vAccel02[i]; }
        // plane A
        VDP_setHorizontalScrollTile( BG_A, 0,  vector04, 28, CPU );
        for ( int i = 0; i < 28; i++ ) { vector04[i] = offsetA; }
    }

    if ( state == 7 ) { // shmup: plane B planet fly-in, plane A parallax
        // plane B: scroll until the planet is roughly centred
        if ( vector05[0] >= -200 ) {
            VDP_setHorizontalScrollTile( BG_B, 0, vector05, 28, CPU );
            for ( int i = 0; i < 28; i++ ) {
                vectorAux[i] = vectorAux[i] + accel04;
                vector05[i]  = F16_toInt( vectorAux[i] );
            }
        }
        // plane A
        VDP_setHorizontalScrollTile( BG_A, 0, vector06, 28, CPU );
        for ( int i = 0; i < 28; i++ ) { vector06[i] -= vAccel03[i]; }

        // move enemies (simplified: placed here to keep the example self-contained)
        for ( int i = 0; i < 6; i++ ) {
            SPR_setPosition( enemies[i], enemyPosx[i] -= 3, enemyPosy[i] );
            // if an enemy exits left, respawn it on the right at a random height
            if ( enemyPosx[i] < -50 ) {
                enemyPosx[i] = 350;
                enemyPosy[i] = ( ( random() % 200 ) - 1 ) + 1;
            }
        }
    }

    if ( state == 8 ) { // vertical tile scroll
        VDP_setVerticalScrollTile( BG_A, 0, vector07, 20, CPU );
        for ( int i = 0; i < 20; i++ ) { vector07[i] -= vAccel04[i]; }
    }

}

static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( state == 1 ) {
        if ( value & BUTTON_A ) { locked = 0; }
    }

    if ( state == 2 ) {
        if ( value & BUTTON_B ) { locked = 0; }
    }

    if ( state == 3 ) {
        if ( value & BUTTON_C ) { locked = 0; }
    }

    if ( state == 4 ) {
        if ( value & BUTTON_A ) { locked = 0; }
    }

    if ( state == 5 ) {
        if ( value & BUTTON_B ) { locked = 0; }
    }

    if ( state == 6 ) { // d-pad: scroll planes and move sprite
        if ( value & BUTTON_LEFT ) {
            offsetA += speedA;
            SPR_setAnim( sonicSprite, ANIM_RUN );
            SPR_setHFlip( sonicSprite, TRUE );
        }
        if ( value & BUTTON_RIGHT ) {
            offsetA -= speedA;
            SPR_setAnim( sonicSprite, ANIM_RUN );
            SPR_setHFlip( sonicSprite, FALSE );
        }
        if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) ) {
            SPR_setAnim( sonicSprite, ANIM_STAND );
        }
        if ( value & BUTTON_C ) { locked = 0; }
    }

    if ( state == 7 ) { // d-pad: move ship with limits | A: next state
        if ( value & BUTTON_LEFT ) {
            sonicPosx -= 2;
            if ( sonicPosx <= LIMIT_LEFT ) { sonicPosx = LIMIT_LEFT; }
            SPR_setAnim( shipSprite, ANIM_SHIP_MOVE );
        }
        if ( value & BUTTON_RIGHT ) {
            sonicPosx += 2;
            if ( sonicPosx >= LIMIT_RIGHT ) { sonicPosx = LIMIT_RIGHT; }
            SPR_setAnim( shipSprite, ANIM_SHIP_MOVE );
        }
        if ( value & BUTTON_UP ) {
            sonicPosy -= 2;
            if ( sonicPosy <= LIMIT_UP ) { sonicPosy = LIMIT_UP; }
            SPR_setAnim( shipSprite, ANIM_SHIP_MOVE );
        }
        if ( value & BUTTON_DOWN ) {
            sonicPosy += 2;
            if ( sonicPosy >= LIMIT_DOWN ) { sonicPosy = LIMIT_DOWN; }
            SPR_setAnim( shipSprite, ANIM_SHIP_MOVE );
        }
        if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) &&
             !( value & BUTTON_UP )    && !( value & BUTTON_DOWN ) ) {
            SPR_setAnim( shipSprite, ANIM_SHIP_IDLE );
        }
        if ( value & BUTTON_A ) { locked = 0; }
        SPR_setPosition( shipSprite, sonicPosx, sonicPosy );
    }

    if ( state == 8 ) { // START: hard reset
        if ( value & BUTTON_START ) { SYS_hardReset(); }
    }

}

static void drawMessages( void ) {

    if ( state == 1 ) {
        VDP_drawText( "Example 01", 4, 12 );
        VDP_drawText( "Press -A- to continue", 4, 13 );
    }

    if ( state == 2 ) {
        VDP_drawText( "Example 02", 4, 12 );
        VDP_drawText( "Press -B- to continue", 4, 13 );
    }

    if ( state == 3 ) {
        VDP_drawText( "Example 03", 4, 12 );
        VDP_drawText( "Press -C- to continue", 4, 13 );
    }

    if ( state == 4 ) {
        VDP_drawText( "Example 04", 4, 12 );
        VDP_drawText( "Press -A- to continue", 4, 13 );
    }

    if ( state == 5 ) {
        VDP_drawText( "Example 05", 4, 12 );
        VDP_drawText( "Press -B- to continue", 4, 13 );
    }

    if ( state == 6 ) {
        VDP_drawText( "Example 06", 4, 12 );
        VDP_drawText( "Press -C- to continue", 4, 13 );
    }

    if ( state == 7 ) {
        VDP_drawTextBG( BG_B, "Example 07", 14, 24 );
        VDP_drawTextBG( BG_B, "Press -A- to continue", 14, 25 );
    }

    if ( state == 8 ) {
        VDP_drawTextBG( BG_B, "Example 08", 7, 24 );
        VDP_drawTextBG( BG_B, "Press -START- to RESET", 7, 25 );
    }

}
