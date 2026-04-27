/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 13 - "The Window Plane" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

/*
    STATE 1: top HUD
    STATE 2: bottom HUD
    STATE 3: left HUD
    STATE 4: right HUD
    STATE 5: top HUD with sprite priority ON  (shadow/highlight active)
    STATE 6: top HUD with sprite priority OFF (shadow/highlight active)
    STATE 7: top + left HUD
    STATE 8: bottom + right HUD
*/

#include <genesis.h>

#include "gfx.h"
#include "sprite.h"
#include <sys.h>

#define ANIM_STAND   0
#define ANIM_RUN     3

#define LIMIT_LEFT   30
#define LIMIT_RIGHT 200
#define LIMIT_UP     30
#define LIMIT_DOWN  200

// function declarations
static void handleInput( void );
static void loadState( void );
static void updateCamera( void );

// Sonic sprite
Sprite *sonicSprite;

// demo state
int state  = 1;
int locked = 0; // inner loop runs while locked==1; set to 0 to advance state

// score values displayed in the HUD
s16  P1score   = 100;
s16  P2score   = 130;
s16  highscore = 10000;
char txt1[8], txt2[8], txt3[8];

// horizontal scroll offsets for each plane
s16 offsetHPlaneA;
s16 offsetHPlaneB;

// scroll and movement speeds
int speedPlaneA = 2;
int speedPlaneB = 1;
int speedSonic  = 3;

// Sonic position
s16 sonicPosx;
s16 sonicPosy;

// HUD tilesets (horizontal = top/bottom bar; vertical = left/right bar)
TileMap *tilesetH, *tilesetV;

// VRAM tile indices: ind = BG images start, ind2 = horizontal HUD start, ind3 = vertical HUD start
u16 ind, ind2, ind3;


int main( bool hard ) {

    state      = 1;
    offsetHPlaneA = 0;
    offsetHPlaneB = 0;
    sonicPosx  = 108;
    sonicPosy  = 158;

    VDP_setScreenWidth320();
    VDP_setScreenHeight224();

    PAL_setPalette( PAL0, bgb_image.palette->data, CPU );
    PAL_setPalette( PAL1, bga_image.palette->data, CPU );
    PAL_setPalette( PAL2, sonic_sprite.palette->data, CPU );
    PAL_setPalette( PAL3, horizontal_hud.palette->data, CPU ); // shared by vertical HUD too

    SPR_init();

    ind = 1; // tile 0 is reserved by SGDK

    VDP_drawImageEx( BG_B, &bgb_image, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bgb_image.tileset->numTile;
    VDP_drawImageEx( BG_A, &bga_image, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bga_image.tileset->numTile;

    // horizontal HUD tileset (top/bottom bar)
    tilesetH = unpackTileMap( horizontal_hud.tilemap, NULL );
    VDP_loadTileSet( horizontal_hud.tileset, ind, CPU );
    ind2 = ind + horizontal_hud.tileset->numTile;

    // vertical HUD tileset (left/right bar)
    tilesetV = unpackTileMap( vertical_hud.tilemap, NULL );
    VDP_loadTileSet( vertical_hud.tileset, ind2, CPU );
    ind3 = ind2 + horizontal_hud.tileset->numTile;

    sonicSprite = SPR_addSprite( &sonic_sprite, sonicPosx, sonicPosy, TILE_ATTR( PAL2, FALSE, FALSE, FALSE ) );

    VDP_setScrollingMode( HSCROLL_PLANE, VSCROLL_PLANE );

    while ( TRUE ) {

        loadState();

        while ( locked ) {

            handleInput();
            updateCamera();

            SPR_update();
            SYS_doVBlankProcess();

        }

        state++;

    }

    return 0;

}


static void loadState( void ) {

    offsetHPlaneA = 0;
    offsetHPlaneB = 0;

    if ( state == 1 ) { // top HUD

        VDP_setWindowVPos( FALSE, 0 ); // clear any previous window plane
        VDP_setHilightShadow( 0 );
        VDP_setWindowVPos( FALSE, 2 ); // window plane: rows 0–1 (2 rows)

        VDP_clearTextArea( 10, 9, 22, 3 );

        VDP_setMapEx( WINDOW, tilesetH, TILE_ATTR_FULL( PAL3, TRUE, FALSE, FALSE, ind ), 0, 0, 0, 0, 40, 2 );

        VDP_setTextPlane( WINDOW );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1,  4, 1 );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2, 20, 1 );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3, 33, 1 );

        VDP_drawTextBG( BG_A, "EXAMPLE: TOP HUD         ", 10,  9 );
        VDP_drawTextBG( BG_B, "Press A for next example ", 10, 10 );

    }

    if ( state == 2 ) { // bottom HUD

        VDP_setWindowVPos( FALSE, 0 );
        VDP_setWindowVPos( TRUE, 26 ); // window plane: rows 25–27

        VDP_clearTextArea( 10, 9, 22, 3 );

        VDP_setMapEx( WINDOW, tilesetH, TILE_ATTR_FULL( PAL3, FALSE, FALSE, FALSE, ind ), 0, 26, 0, 0, 40, 2 );

        VDP_setTextPlane( WINDOW );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1,  4, 27 );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2, 20, 27 );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3, 33, 27 );

        VDP_drawTextBG( BG_A, "EXAMPLE: BOTTOM HUD      ", 10,  9 );
        VDP_drawTextBG( BG_B, "Press B for next example ", 10, 10 );

    }

    if ( state == 3 ) { // left HUD

        VDP_setWindowVPos( FALSE, 0 );
        VDP_setWindowHPos( FALSE, 2 ); // window plane: columns 0–2

        VDP_clearTextArea( 10, 9, 22, 3 );

        VDP_setMapEx( WINDOW, tilesetV, TILE_ATTR_FULL( PAL3, FALSE, FALSE, FALSE, ind2 ), 0, 0, 0, 0, 4, 28 );

        VDP_setTextPlane( WINDOW );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1, 0,  4 );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2, 0, 13 );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3, 0, 23 );

        VDP_drawTextBG( BG_A, "EXAMPLE: LEFT HUD        ", 10,  9 );
        VDP_drawTextBG( BG_B, "Press C for next example ", 10, 10 );

    }

    if ( state == 4 ) { // right HUD

        VDP_setWindowVPos( FALSE, 0 );
        VDP_setWindowHPos( TRUE, 18 ); // window plane: column 36 (18×2) to end

        VDP_clearTextArea( 10, 9, 22, 3 );

        VDP_setMapEx( WINDOW, tilesetV, TILE_ATTR_FULL( PAL3, FALSE, FALSE, FALSE, ind2 ), 36, 0, 0, 0, 4, 28 );

        VDP_setTextPlane( WINDOW );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1, 37,  4 );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2, 37, 13 );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3, 37, 23 );

        VDP_drawTextBG( BG_A, "EXAMPLE: RIGHT HUD       ", 10,  9 );
        VDP_drawTextBG( BG_B, "Press A for next example ", 10, 10 );

    }

    if ( state == 5 ) { // top HUD — sprite priority ON (shadow/highlight active)

        VDP_setWindowHPos( FALSE, 0 );
        VDP_setWindowVPos( FALSE, 2 );

        VDP_clearTextArea( 10, 9, 22, 3 );

        VDP_setMapEx( WINDOW, tilesetH, TILE_ATTR_FULL( PAL3, TRUE, FALSE, FALSE, ind ), 0, 0, 0, 0, 40, 2 );

        VDP_setTextPlane( WINDOW );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1,  4, 1 );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2, 20, 1 );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3, 33, 1 );

        VDP_drawTextBG( BG_A, "TOP HUD PRIORITY = ON    ", 10,  9 );
        VDP_drawTextBG( BG_B, "Shadow/Highlight active  ", 10, 10 );
        VDP_drawTextBG( BG_B, "Press B for next example ", 10, 10 );

        VDP_setHilightShadow( 1 );

    }

    if ( state == 6 ) { // top HUD — sprite priority OFF (shadow/highlight active)

        VDP_setWindowVPos( FALSE, 0 );
        VDP_setWindowVPos( FALSE, 2 );

        VDP_clearTextArea( 10, 9, 22, 3 );

        VDP_setMapEx( WINDOW, tilesetH, TILE_ATTR_FULL( PAL3, FALSE, FALSE, FALSE, ind ), 0, 0, 0, 0, 40, 2 );

        VDP_setTextPlane( WINDOW );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1,  4, 1 );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2, 20, 1 );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3, 33, 1 );

        VDP_drawTextBG( BG_A, "TOP HUD PRIORITY = OFF   ", 10,  9 );
        VDP_drawTextBG( BG_B, "Shadow/Highlight active  ", 10, 10 );
        VDP_drawTextBG( BG_B, "Press C for next example ", 10, 10 );

    }

    if ( state == 7 ) { // top + left HUD

        VDP_setWindowVPos( FALSE, 0 );
        VDP_setHilightShadow( 0 );

        VDP_setWindowVPos( FALSE, 2 ); // rows 0–1
        VDP_setWindowHPos( FALSE, 2 ); // columns 0–2

        VDP_clearTextArea( 10, 9, 22, 3 );

        VDP_setMapEx( WINDOW, tilesetH, TILE_ATTR_FULL( PAL3, TRUE,  FALSE, FALSE, ind  ), 0, 0, 0, 0, 40,  2 );
        VDP_setMapEx( WINDOW, tilesetV, TILE_ATTR_FULL( PAL3, FALSE, FALSE, FALSE, ind2 ), 0, 0, 0, 0,  4, 28 );

        VDP_setTextPlane( WINDOW );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1,  4, 1  );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2, 20, 1  );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3, 33, 1  );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1,  0,  4 );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2,  0, 13 );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3,  0, 23 );

        VDP_drawTextBG( BG_A, "HUD: TOP + LEFT          ", 10,  9 );
        VDP_drawTextBG( BG_B, "Press A for next example ", 10, 10 );

    }

    if ( state == 8 ) { // bottom + right HUD

        VDP_setWindowVPos( FALSE, 0 );
        VDP_setWindowHPos( FALSE, 0 );

        VDP_setWindowVPos( TRUE, 26 ); // rows 25–27
        VDP_setWindowHPos( TRUE, 18 ); // column 36 (18×2) to end

        VDP_clearTextArea( 10, 9, 22, 3 );

        VDP_setMapEx( WINDOW, tilesetH, TILE_ATTR_FULL( PAL3, FALSE, FALSE, FALSE, ind  ), 0,  26, 0, 0, 40,  2 );
        VDP_setMapEx( WINDOW, tilesetV, TILE_ATTR_FULL( PAL3, FALSE, FALSE, FALSE, ind2 ), 36,  0, 0, 0,  4, 28 );

        VDP_setTextPlane( WINDOW );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1, 37,  4 );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2, 37, 13 );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3, 37, 23 );
        intToStr( P1score,   txt1, 0 ); VDP_drawText( txt1,  4, 27 );
        intToStr( highscore, txt2, 0 ); VDP_drawText( txt2, 20, 27 );
        intToStr( P2score,   txt3, 0 ); VDP_drawText( txt3, 33, 27 );

        VDP_drawTextBG( BG_A, "HUD: BOTTOM + RIGHT      ", 10,  9 );
        VDP_drawTextBG( BG_B, "Press B to reset         ", 10, 10 );

    }

    locked = 1;

}


static void updateCamera( void ) {

    VDP_setHorizontalScroll( BG_A, offsetHPlaneA );
    VDP_setHorizontalScroll( BG_B, offsetHPlaneB );

}


static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

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

    if ( value & BUTTON_UP )   { sonicPosy--; }
    if ( value & BUTTON_DOWN ) { sonicPosy++; }

    SPR_setPosition( sonicSprite, sonicPosx, sonicPosy );

    if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) ) {
        SPR_setAnim( sonicSprite, ANIM_STAND );
    }

    if ( state == 1 ) { if ( value & BUTTON_A ) { locked = 0; } }
    if ( state == 2 ) { if ( value & BUTTON_B ) { locked = 0; } }
    if ( state == 3 ) { if ( value & BUTTON_C ) { locked = 0; } }
    if ( state == 4 ) { if ( value & BUTTON_A ) { locked = 0; } }
    if ( state == 5 ) { if ( value & BUTTON_B ) { locked = 0; } }
    if ( state == 6 ) { if ( value & BUTTON_C ) { locked = 0; } }
    if ( state == 7 ) { if ( value & BUTTON_A ) { locked = 0; } }

    if ( state == 8 ) {
        if ( value & BUTTON_B ) { SYS_reset(); }
    }

    if ( value & BUTTON_START ) { SYS_reset(); }

}
