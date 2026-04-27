/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 09 - "Scroll (2) - By Lines" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

/*
    STATE 1: line scroll — various effects (clouds + sea)
    STATE 2: SF2-style perspective floor with Ryu sprite
    STATE 3: Sega logo horizontal deformation (diagonal warp)
    STATE 4: line-separation effect (alternating lines drift apart)
*/

#include <genesis.h>

#include "gfx.h"
#include "sprite.h"
#include <sys.h>

// Ryu animations (state 2)
#define ANIM_STAND  0
#define ANIM_WALK   1

// function declarations
static void handleInput( void );
static void loadState( void );
static void drawMessages( void );
static void updateCamera( void );

// tile counter in VRAM
u16 ind;

// demo state — increments each time the user advances to the next example
int state  = 1;
int locked = 0; // inner loop runs while locked==1; set to 0 to advance state

// state 1: scroll vectors for plane A (full) and two plane B regions
s16   offsetA        = 0;
s16   vectorA[224];
s16   vectorB1[50];          // plane B rows 0..49 (clouds)
s16   vectorB2[80];          // plane B rows 144..223 (sea)
fix16 vectorB2Aux[80];
fix16 vectorB2Accel = FIX16( 0.6 );

// state 2: Ryu sprite
Sprite *ryuSprite;
u16    ryuPosx = 160;
u16    ryuPosy = 120;

#define LIMIT_LEFT   70
#define LIMIT_RIGHT 260

// state 2: plane B slow horizontal drift
fix16 offsetB;
s16   vectorB[224];
fix16 vectorBAux[224];

#define BG_B_SCROLL_SPEED     FIX16(  0.65 )
#define BG_B_SCROLL_SPEED_NEG FIX16( -0.65 )

// state 2: SF2-style floor scroll vectors
//   vectorS      — integer scroll values passed to VDP_setHorizontalScrollLine
//   vectorSAccel — per-row acceleration; top rows move slower than bottom rows
//   vectorSAux   — sub-pixel accumulation buffer
s16   vectorS[40];
fix16 vectorSAccel[40] = {
     0.8,  1.0,  1.2,  1.6,  2.0,   2.4,  2.8,  3.2,  3.6,  4.0,
     4.4,  4.8,  5.2,  5.6,  6.0,   6.4,  6.8,  7.2,  7.6,  8.0,
     8.4,  8.8,  9.2,  9.6, 10.0,  10.4, 10.8, 11.2, 11.6, 12.0,
    12.0, 12.0, 12.0, 12.0, 12.0,  12.0, 12.0, 12.0, 12.0, 12.0
};
fix16 vectorSAux[40];

// state 3: diagonal scroll table helpers
#define NUM_COLUMNS  40
#define NUM_ROWS     28
#define NUM_LINES    ( NUM_ROWS * 8 )

#define InitializeScrollTable() \
    for ( u16 i = 0; i < NUM_LINES; i++ ) { line_scroll_data[i] = FIX16( 0 ); }

#define InitializeSpeedTable() \
    line_speed_data[0] = FIX16( 0 ); \
    for ( u16 i = 1; i < NUM_LINES; i++ ) { \
        line_speed_data[i] = line_speed_data[i - 1] + FIX16( 1 ); \
    }

#define InitializeSpeedTable_dynamic() \
    line_speed_data[0] = FIX16( 0 ); \
    for ( u16 i = 1; i < NUM_LINES; i++ ) { \
        line_speed_data[i] = line_speed_data[i - 1] + FIX16( 1 ); \
        line_speed_data[i] = line_speed_data[i - 1] + dynamicDiagonal; \
    }

fix16 dynamicDiagonal = FIX16( 0 );

// state 4: line-separation vectors (even/odd rows drift in opposite directions)
s16   vSeparation[224];
fix16 vSeparationAccel[224];
fix16 vSeparationAux[224];


int main( bool hard ) {

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


static void loadState( void ) {

    SPR_end();
    VDP_resetScreen();
    VDP_init();

    if ( state == 1 ) {

        offsetA = 0;
        for ( int i = 0; i < 224; i++ ) { vectorA[i]     = 0; }
        for ( int i = 0; i < 50;  i++ ) { vectorB1[i]    = 0; }
        for ( int i = 0; i < 80;  i++ ) { vectorB2[i]    = 0; }
        for ( int i = 0; i < 80;  i++ ) { vectorB2Aux[i] = FIX16( 0 ); }

        VDP_setScreenWidth320();
        VDP_setScreenHeight224();

        PAL_setPalette( PAL0, bgb_image.palette->data, CPU );
        PAL_setPalette( PAL1, bga_image.palette->data, CPU );
        PAL_setPalette( PAL2, ryu_sprite.palette->data, CPU );

        SPR_init();

        ind = TILE_USER_INDEX;
        VDP_drawImageEx( BG_B, &bgb_image, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
        ind += bgb_image.tileset->numTile;
        VDP_drawImageEx( BG_A, &bga_image, TILE_ATTR_FULL( PAL1, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
        ind += bga_image.tileset->numTile;

        VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE );

    }

    if ( state == 2 ) {

        offsetA = 0;
        offsetB = FIX16( 0 );
        for ( int i = 0; i < 224; i++ ) { vectorA[i]    = -96; }
        for ( int i = 0; i < 224; i++ ) { vectorB[i]    = FIX16( 0 ); }
        for ( int i = 0; i < 224; i++ ) { vectorBAux[i] = FIX16( -56 ); }
        for ( int i = 0; i < 40;  i++ ) { vectorS[i]    = -96; }
        for ( int i = 0; i < 40;  i++ ) { vectorSAux[i] = FIX16( -96.0 ); }

        VDP_setScreenWidth320();
        VDP_setScreenHeight224();

        PAL_setPalette( PAL0, bgc_image.palette->data, CPU );
        PAL_setPalette( PAL1, ryu_sprite.palette->data, CPU );

        SPR_init();

        ind = TILE_USER_INDEX;
        VDP_drawImageEx( BG_B, &bgd_image, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
        ind += bgd_image.tileset->numTile;
        VDP_drawImageEx( BG_A, &bgc_image, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
        ind += bgc_image.tileset->numTile;

        VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE );

        ryuSprite = SPR_addSprite( &ryu_sprite, ryuPosx, ryuPosy, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ) );
        SPR_setHFlip( ryuSprite, TRUE );

    }

    if ( state == 3 ) {

        VDP_setScreenWidth320();
        VDP_setScreenHeight224();

        PAL_setPalette( PAL0, bge_image.palette->data, CPU );

        SPR_init();

        ind = TILE_USER_INDEX;
        VDP_drawImageEx( BG_A, &bge_image, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
        ind += bge_image.tileset->numTile;

        VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE );

    }

    if ( state == 4 ) {

        for ( int i = 0; i < 224; i++ ) { vSeparation[i]    = 0; }
        for ( int i = 0; i < 224; i++ ) {
            vSeparationAccel[i] = ( i % 2 ) ? FIX16( 0.05 ) : FIX16( -0.05 );
        }
        for ( int i = 0; i < 224; i++ ) { vSeparationAux[i] = FIX16( 0 ); }

        VDP_setScreenWidth320();
        VDP_setScreenHeight224();

        SPR_init();

        PAL_setPalette( PAL0, bge_image.palette->data, CPU );

        ind = TILE_USER_INDEX;
        VDP_drawImageEx( BG_A, &bge_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
        ind += bge_image.tileset->numTile;

        VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE );

    }

    locked = 1;

}


static void updateCamera( void ) {

    if ( state == 1 ) {

        // plane A — uniform horizontal scroll across all lines
        for ( int i = 0; i < 224; i++ ) { vectorA[i] = offsetA; }
        VDP_setHorizontalScrollLine( BG_A, 0, vectorA, 224, CPU );
        offsetA--;

        // plane B rows 0..49 — clouds (constant speed)
        for ( int i = 0; i < 50; i++ ) { vectorB1[i] -= 2; }
        VDP_setHorizontalScrollLine( BG_B, 0, vectorB1, 50, CPU );

        // plane B rows 144..223 — sea (accelerating per row)
        for ( int i = 0; i < 80; i++ ) {
            vectorB2Aux[i] = vectorB2Aux[i] - vectorB2Accel + i;
            vectorB2[i]    = F16_toInt( vectorB2Aux[i] );
        }
        VDP_setHorizontalScrollLine( BG_B, 144, vectorB2, 80, CPU );

    }

    if ( state == 2 ) {

        // plane A — rows 0..179 (sky/background above the floor)
        for ( int i = 0; i < 180; i++ ) { vectorA[i] += offsetA; }
        VDP_setHorizontalScrollLine( BG_A, 0, vectorA, 180, CPU );

        // plane A — rows 180..219 (floor, perspective scroll)
        VDP_setHorizontalScrollLine( BG_A, 180, vectorS, 40, CPU );

        // plane B — full height slow drift
        for ( int i = 0; i < 224; i++ ) {
            vectorBAux[i] = vectorBAux[i] + offsetB;
            vectorB[i]    = F16_toInt( vectorBAux[i] );
        }
        VDP_setHorizontalScrollLine( BG_B, 0, vectorB, 224, CPU );

    }

    if ( state == 4 ) {

        VDP_setHorizontalScrollLine( BG_A, 0, vSeparation, 224, CPU );

    }

}


static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( state == 1 ) {
        if ( value & BUTTON_A ) { locked = 0; }
    }

    if ( state == 2 ) {

        if ( value & BUTTON_B ) { locked = 0; }

        if ( value & BUTTON_LEFT ) {

            if ( ryuPosx >= LIMIT_LEFT ) {
                SPR_setPosition( ryuSprite, ryuPosx -= 2, ryuPosy );
                SPR_setAnim( ryuSprite, ANIM_WALK );
                SPR_setHFlip( ryuSprite, FALSE );
                offsetA = +1;
                offsetB = BG_B_SCROLL_SPEED;

                for ( int i = 0; i < 40; i++ ) {
                    vectorSAux[i] = vectorSAux[i] - vectorSAccel[i]; // per-row deceleration
                    vectorSAux[i] = vectorSAux[i] + FIX16( +1 );     // plus background offset
                    vectorS[i]    = F16_toInt( vectorSAux[i] );
                }

            } else {
                offsetA = 0;
                offsetB = FIX16( 0 );
            }

        }

        if ( value & BUTTON_RIGHT ) {

            if ( ryuPosx <= LIMIT_RIGHT ) {
                SPR_setPosition( ryuSprite, ryuPosx += 2, ryuPosy );
                SPR_setAnim( ryuSprite, ANIM_WALK );
                SPR_setHFlip( ryuSprite, TRUE );
                offsetA = -1;
                offsetB = BG_B_SCROLL_SPEED_NEG;

                for ( int i = 0; i < 40; i++ ) {
                    vectorSAux[i] = vectorSAux[i] + vectorSAccel[i]; // per-row acceleration
                    vectorSAux[i] = vectorSAux[i] + FIX16( -1 );     // plus background offset
                    vectorS[i]    = F16_toInt( vectorSAux[i] );
                }

            } else {
                offsetA = 0;
                offsetB = FIX16( 0 );
            }

        }

        if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) ) {
            SPR_setAnim( ryuSprite, ANIM_STAND );
            offsetA = 0;
            offsetB = FIX16( 0 );
        }

    }

    if ( state == 3 ) {

        if ( value & BUTTON_C ) { locked = 0; }

        if ( value & BUTTON_RIGHT ) {

            dynamicDiagonal = dynamicDiagonal + FIX16( 0.025 );

            fix16 line_scroll_data[NUM_LINES];
            fix16 line_speed_data[NUM_LINES];
            s16   aux[NUM_LINES];

            InitializeScrollTable();
            InitializeSpeedTable_dynamic();

            for ( u16 i = 0; i < NUM_LINES; i++ ) {
                line_scroll_data[i] = line_scroll_data[i] + line_speed_data[i];
                aux[i] = F16_toInt( line_scroll_data[i] );
            }

            VDP_setHorizontalScrollLine( BG_A, 0, aux, NUM_LINES, 1 );

        }

        if ( value & BUTTON_LEFT ) {

            dynamicDiagonal = dynamicDiagonal - FIX16( 0.025 );

            fix16 line_scroll_data[NUM_LINES];
            fix16 line_speed_data[NUM_LINES];
            s16   aux[NUM_LINES];

            InitializeScrollTable();
            InitializeSpeedTable_dynamic();

            for ( u16 i = 0; i < NUM_LINES; i++ ) {
                line_scroll_data[i] = line_scroll_data[i] + line_speed_data[i];
                aux[i] = F16_toInt( line_scroll_data[i] );
            }

            VDP_setHorizontalScrollLine( BG_A, 0, aux, NUM_LINES, 1 );

        }

    }

    if ( state == 4 ) {

        if ( value & BUTTON_START ) { SYS_hardReset(); }

        if ( value & BUTTON_RIGHT ) {
            for ( int i = 0; i < 224; i++ ) {
                vSeparationAux[i] = vSeparationAux[i] + vSeparationAccel[i];
                vSeparation[i]    = F16_toInt( vSeparationAux[i] );
            }
        }

        if ( value & BUTTON_LEFT ) {
            for ( int i = 0; i < 224; i++ ) {
                vSeparationAux[i] = vSeparationAux[i] - vSeparationAccel[i];
                vSeparation[i]    = F16_toInt( vSeparationAux[i] );
            }
        }

    }

}


static void drawMessages( void ) {

    if ( state == 1 ) {
        VDP_drawText( "Example 01: line scroll effects", 4, 10 );
        VDP_drawText( "Press -A- to continue          ", 4, 11 );
    }

    if ( state == 2 ) {
        VDP_drawText( "Example 02: SF2-style floor    ", 4, 10 );
        VDP_drawText( "LEFT/RIGHT: move               ", 4, 12 );
        VDP_drawText( "Press -B- to continue          ", 4, 13 );
    }

    if ( state == 3 ) {
        VDP_drawText( "Example 03: Sega logo warp     ", 10, 22 );
        VDP_drawText( "LEFT/RIGHT: adjust diagonal    ", 10, 23 );
        VDP_drawText( "Press -C- to continue          ", 10, 24 );
    }

    if ( state == 4 ) {
        VDP_drawText( "Example 04: line separation    ", 10, 22 );
        VDP_drawText( "LEFT/RIGHT: spread lines       ", 10, 23 );
        VDP_drawText( "Press -START- to reset         ", 10, 24 );
    }

}
