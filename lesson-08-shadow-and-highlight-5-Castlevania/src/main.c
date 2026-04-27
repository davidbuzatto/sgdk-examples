/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 08 - "Shadow and Highlight (5) - Castlevania" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

#include "backgrounds.h"
#include "sprites.h"
#include "tools.h"
#include "joy.h"

// Morris animations
#define ANIM_STAND  0
#define ANIM_RUN    1

// Death animations (one per palette color index used)
#define ANIM_DEATH_NORMAL   0
#define ANIM_DEATH_COLOR13  1
#define ANIM_DEATH_COLOR14  2
#define ANIM_DEATH_COLOR15  3

// priority map covers 2 screens wide = 80x28 tiles
#define SCREEN_WIDTH_TILES    80
#define SCREEN_HEIGHT_TILES   28

#define SCENARIO_POS_X        0
#define SCENARIO_POS_Y        0

#define SCENARIO_WIDTH_TILES  SCREEN_WIDTH_TILES
#define SCENARIO_HEIGHT_TILES SCREEN_HEIGHT_TILES

// total tiles in the priority map = 80 x 28 = 2240
#define SCENARIO_NUM_TILES    ( SCENARIO_WIDTH_TILES * SCENARIO_HEIGHT_TILES )

// line-scroll table dimensions
#define NUM_COLUMNS  40
#define NUM_ROWS     28
#define NUM_LINES    ( NUM_ROWS * 8 )

// zero out all entries in line_scroll_data[]
#define InitializeScrollTable() \
    for ( i = 0; i < NUM_LINES; i++ ) { line_scroll_data[i] = FIX16( 0 ); }

// fill line_speed_data[] with the sequence 0, 1, 2, 3, ..., (NUM_LINES-1)
#define InitializeSpeedTable() \
    line_speed_data[0] = FIX16( 0 ); \
    for ( i = 1; i < NUM_LINES; i++ ) { \
        line_speed_data[i] = line_speed_data[i - 1] + FIX16( 1 ); \
    }

// fill line_speed_data[] with a dynamic (uniform) diagonal value
#define InitializeSpeedTable_dynamic() \
    line_speed_data[0] = FIX16( 0 ); \
    for ( i = 1; i < NUM_LINES; i++ ) { \
        line_speed_data[i] = line_speed_data[i - 1] = FIX16( 1 ); \
        line_speed_data[i] = line_speed_data[i - 1] = dynamicDiagonal; \
    }

// current diagonal value for the dynamic scroll demo
fix16 dynamicDiagonal = FIX16( 1 );

// function declarations
static void handleInput( void );
static void moveEnemies( void );
static void drawMessages( void );
void myJoyHandler( u16 joy, u16 changed, u16 state );

// Morris sprite
Sprite *morrisSprite;

// Morris position on screen
int morrisX = 64;
int morrisY = 152;

// Death sprite
Sprite *deathSprite;

// Death position on screen
int deathX = 220;
int deathY = 50;
// int enemyDirection; // OPTIONAL: see moveEnemies()

// sinusoidal movement parameters for Death
#define DEATH_Y_MIDPOINT 70
int moveSpeed  = 1;
int sineSpeed  = 1;
int amplitude  = 1;

// current demo state
int state = 0;

// tile counter in VRAM
u16 ind;

int main( bool hard ) {

    // set display resolution to 320x224
    VDP_setScreenWidth320();

    // initialize the sprite engine
    SPR_init();

    // load background palettes
    PAL_setPalette( PAL0, bg1.palette->data, CPU );
    PAL_setPalette( PAL1, bg2.palette->data, CPU );

    // load both backgrounds into the VDP
    ind = TILE_USER_INDEX;
    VDP_drawImageEx( BG_B, &bg1, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bg1.tileset->numTile;
    VDP_drawImageEx( BG_A, &bg2, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bg2.tileset->numTile;

    // load the Morris palette into PAL2
    PAL_setPalette( PAL2, morris_sprite.palette->data, CPU );

    // add Morris sprite
    morrisSprite = SPR_addSprite( &morris_sprite, morrisX, morrisY, TILE_ATTR( PAL2, FALSE, FALSE, FALSE ) );

    // OPTIONAL: enemigoSentidoMovimiento = 1;

    // load the Death palette into PAL3
    PAL_setPalette( PAL3, death_sprite.palette->data, CPU );

    // add Death sprite
    deathSprite = SPR_addSprite( &death_sprite, deathX, deathY, TILE_ATTR( PAL3, FALSE, FALSE, FALSE ) );

    // set Death's initial animation
    SPR_setAnim( deathSprite, ANIM_DEATH_NORMAL );

    // register the asynchronous joypad handler
    JOY_init();
    JOY_setEventHandler( &myJoyHandler );

    // use PAL3 for text (SGDK always reads color index 15 from the active text palette)
    VDP_setTextPalette( PAL3 );

    SPR_update(); // recommended initial flush

    while ( TRUE ) {

        // synchronous input — move Morris
        handleInput();

        // move enemy sprites
        moveEnemies();

        // display state-based help text
        drawMessages();

        // flush sprite updates to the VDP
        SPR_update();

        // sync with the TV vertical blank
        SYS_doVBlankProcess();

    }

    return 0;

}

// synchronous input — reads joypad and moves Morris
// Horizontal movement is handled here (not in the async handler) because
// the async handler only fires on state changes; holding a button would
// only trigger once and then stop updating the position.
static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        SPR_setPosition( morrisSprite, morrisX--, morrisY );
        SPR_setHFlip( morrisSprite, TRUE );
        SPR_setAnim( morrisSprite, ANIM_RUN );
    }

    if ( value & BUTTON_RIGHT ) {
        SPR_setPosition( morrisSprite, morrisX++, morrisY );
        SPR_setHFlip( morrisSprite, FALSE );
        SPR_setAnim( morrisSprite, ANIM_RUN );
    }

    // no horizontal input — return to stand animation
    if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) ) {
        SPR_setAnim( morrisSprite, ANIM_STAND );
    }

    // STATE 12: dynamic diagonal scroll — UP/DOWN adjust the diagonal value
    if ( state == 12 ) {

        if ( value & BUTTON_UP ) {
            dynamicDiagonal = dynamicDiagonal + FIX16( 0.025 );

            u16 i;
            fix16 line_scroll_data[NUM_LINES];
            fix16 line_speed_data[NUM_LINES];
            s16 aux[NUM_LINES];

            InitializeScrollTable();
            InitializeSpeedTable_dynamic();
            VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE );

            for ( i = 0; i < NUM_LINES; i++ ) {
                line_scroll_data[i] = line_scroll_data[i] + line_speed_data[i];
                aux[i] = F16_toInt( line_scroll_data[i] );
            }

            VDP_setHorizontalScrollLine( BG_A, 0, aux, NUM_LINES, 1 );
        }

        if ( value & BUTTON_DOWN ) {
            dynamicDiagonal = dynamicDiagonal - FIX16( 0.025 );

            u16 i;
            fix16 line_scroll_data[NUM_LINES];
            fix16 line_speed_data[NUM_LINES];
            s16 aux[NUM_LINES];

            InitializeScrollTable();
            InitializeSpeedTable_dynamic();
            VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE );

            for ( i = 0; i < NUM_LINES; i++ ) {
                line_scroll_data[i] = line_scroll_data[i] + line_speed_data[i];
                aux[i] = F16_toInt( line_scroll_data[i] );
            }

            VDP_setHorizontalScrollLine( BG_A, 0, aux, NUM_LINES, 1 );
        }

    }

}

// asynchronous input — A/B/C advance through the shadow/highlight demo states
void myJoyHandler( u16 joy, u16 changed, u16 joyState ) {

    if ( joy == JOY_1 ) {

        // STATE 0: shadow/highlight OFF — everything renders normally

        // STATE 1: enable S&H — all low-priority sprites appear in SHADOW mode
        if ( state == 0 && ( changed & BUTTON_A ) ) {
            state = 1;
            VDP_setHilightShadow( 1 );
        }

        // STATE 2: give Morris high priority (renders at normal brightness)
        if ( state == 1 && ( changed & BUTTON_B ) ) {
            state = 2;
            SPR_setPriority( morrisSprite, TRUE );
        }

        // STATE 3: give Death high priority as well
        if ( state == 2 && ( changed & BUTTON_C ) ) {
            state = 3;
            SPR_setPriority( deathSprite, TRUE );
        }

        // STATE 4: draw only the moon sub-image with high priority on plane B
        // (only those specific tiles get priority, not the entire background)
        if ( state == 3 && ( changed & BUTTON_A ) ) {
            state = 4;
            VDP_drawImageEx( BG_B, &bg3, TILE_ATTR_FULL( PAL1, TRUE, FALSE, FALSE, ind ), 14, 4, FALSE, TRUE );
            ind += bg3.tileset->numTile;
        }

        // STATE 5: redraw both backgrounds with high priority — everything at normal brightness
        if ( state == 4 && ( changed & BUTTON_B ) ) {
            state = 5;
            ind = TILE_USER_INDEX;
            VDP_drawImageEx( BG_B, &bg1, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
            ind += bg1.tileset->numTile;
            VDP_drawImageEx( BG_A, &bg2, TILE_ATTR_FULL( PAL1, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
            ind += bg2.tileset->numTile;
        }

        // STATE 6: remove all priorities; switch Death to color-index-13 animation
        // color 13 of PAL3 appears in shadow without priority, normal with priority
        if ( state == 5 && ( changed & BUTTON_C ) ) {
            state = 6;
            ind = TILE_USER_INDEX;
            VDP_drawImageEx( BG_B, &bg1, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
            ind += bg1.tileset->numTile;
            VDP_drawImageEx( BG_A, &bg2, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
            ind += bg2.tileset->numTile;
            SPR_setPriority( morrisSprite, FALSE );
            SPR_setAnim( deathSprite, ANIM_DEATH_COLOR13 );
            SPR_setPriority( morrisSprite, FALSE );
        }

        // STATE 7: give Morris high priority — color 13 appears at normal brightness
        if ( state == 6 && ( changed & BUTTON_A ) ) {
            state = 7;
            SPR_setAnim( deathSprite, ANIM_DEATH_COLOR13 );
            SPR_setPriority( morrisSprite, TRUE );
        }

        // STATE 8: switch Death to color index 14 (highlight — bright + transparent)
        if ( state == 7 && ( changed & BUTTON_B ) ) {
            state = 8;
            SPR_setAnim( deathSprite, ANIM_DEATH_COLOR14 );
        }

        // STATE 9: switch Death to color index 15 (shadow — dark + transparent)
        if ( state == 8 && ( changed & BUTTON_C ) ) {
            state = 9;
            SPR_setAnim( deathSprite, ANIM_DEATH_COLOR15 );
        }

        // STATE 10: reset everything and start the priority-map demo
        if ( state == 9 && ( changed & BUTTON_A ) ) {
            state = 10;

            SPR_reset();
            VDP_clearPlane( BG_A, TRUE );
            VDP_clearPlane( BG_B, TRUE );

            // re-initialise from scratch
            VDP_setScreenWidth320();
            SPR_init();

            PAL_setPalette( PAL2, morris_sprite.palette->data, CPU );
            morrisSprite = SPR_addSprite( &morris_sprite, morrisX, morrisY, TILE_ATTR( PAL2, FALSE, FALSE, FALSE ) );

            deathSprite = SPR_addSprite( &death_sprite, deathX, deathY, TILE_ATTR( PAL3, FALSE, FALSE, FALSE ) );
            SPR_setAnim( deathSprite, ANIM_DEATH_NORMAL );

            // reset sprite positions in case the user moved them off-screen
            morrisX = 64;
            morrisY = 152;
            deathX  = 220;
            deathY  = 50;

            // build the priority tilemap buffer
            u16 tilemapBuffer[SCENARIO_NUM_TILES];
            u16 *planeTilemap = &tilemapBuffer[0];

            for ( int j = 0; j < SCENARIO_NUM_TILES; j++ ) {
                tilemapBuffer[j] = 0;
            }

            // pointer to the actual u16 tile-index array inside the priority map image
            // (bg_double_priority.tilemap is a TileMap struct; ->tilemap is the u16 data)
            const u16 *shadowTileData = bg_double_priority.tilemap->tilemap;
            u16 numTiles = SCENARIO_NUM_TILES;

            // Walk every tile. If the priority-map tile is non-zero, set the
            // priority bit on the corresponding plane A tile.
            // TILE_ATTR_PRIORITY_MASK = 0x8000; tilemapBuffer was zeroed above.
            while ( numTiles-- ) {

                if ( *shadowTileData ) {
                    *planeTilemap |= TILE_ATTR_PRIORITY_MASK;
                }

                planeTilemap++;
                shadowTileData++;  // u16 pointer — advances exactly 2 bytes (one tile entry)

            }

            // load the real background into plane B
            PAL_setPalette( PAL0, bg_double.palette->data, CPU );

            ind = TILE_USER_INDEX;
            VDP_drawImageEx( BG_B, &bg_double, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
            ind += bg_double.tileset->numTile;

            // upload the priority tilemap to plane A
            VDP_setTileMapDataRectEx(
                BG_A, &tilemapBuffer[0], 0,
                SCENARIO_POS_X, SCENARIO_POS_Y,
                SCENARIO_WIDTH_TILES, SCENARIO_HEIGHT_TILES, SCENARIO_WIDTH_TILES, CPU
            );
            ind += bg_double_priority.tileset->numTile;

            VDP_setHilightShadow( 1 );
        }

        // STATE 11: static diagonal scroll effect
        if ( state == 10 && ( changed & BUTTON_B ) ) {
            state = 11;

            u16 i;
            fix16 line_scroll_data[NUM_LINES];
            fix16 line_speed_data[NUM_LINES];
            s16 aux[NUM_LINES];

            InitializeScrollTable();
            InitializeSpeedTable();
            VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE );

            for ( i = 0; i < NUM_LINES; i++ ) {
                line_scroll_data[i] = line_scroll_data[i] + line_speed_data[i];
                aux[i] = F16_toInt( line_scroll_data[i] );
            }

            // tell the VDP how many pixels to offset each scan line on plane A,
            // starting from line 0, for NUM_LINES lines;
            // mode 1 = CPU transfer (during the non-display period)
            VDP_setHorizontalScrollLine( BG_A, 0, aux, NUM_LINES, 1 );
            // note: scrolling the entire plane also shifts the text drawn on BG_A
        }

        // STATE 12: dynamic diagonal scroll — UP/DOWN keys adjust the angle
        if ( state == 11 && ( changed & BUTTON_C ) ) {
            state = 12;
        }

    }

}

// display state-specific instructions and status text
static void drawMessages( void ) {

    if ( state == 0 ) {
        VDP_drawText( "S&H: OFF                               ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON: OFF, RUINS: OFF      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS: OFF, DEATH: OFF   ", 1, 2 );
        VDP_drawText( "-A- Enable S&H                         ", 1, 26 );
        VDP_drawText( "                                       ", 1, 27 );
    }

    if ( state == 1 ) {
        VDP_drawText( "S&H: ON                                ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON: OFF, RUINS: OFF      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS: OFF, DEATH: OFF   ", 1, 2 );
        VDP_drawText( "-B- Enable priority on Morris          ", 1, 26 );
        VDP_drawText( "In S/H: Priority=front=normal color    ", 1, 27 );
    }

    if ( state == 2 ) {
        VDP_drawText( "S&H: ON                                ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON: OFF, RUINS: OFF      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS:  ON, DEATH: OFF   ", 1, 2 );
        VDP_drawText( "-C- Enable priority on Death           ", 1, 26 );
        VDP_drawText( "                                       ", 1, 27 );
    }

    if ( state == 3 ) {
        VDP_drawText( "S&H: ON                                ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON: OFF, RUINS: OFF      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS:  ON, DEATH:  ON   ", 1, 2 );
        VDP_drawText( "-A- Load cropped moon at correct pos   ", 1, 26 );
        VDP_drawText( "and set those tiles to high priority   ", 1, 27 );
    }

    if ( state == 4 ) {
        VDP_drawText( "S&H: ON                                ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON:  ON, RUINS: OFF      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS:  ON, DEATH:  ON   ", 1, 2 );
        VDP_drawText( "Plane A tiles overlapping the moon     ", 1, 25 );
        VDP_drawText( "inherit its priority from plane B      ", 1, 26 );
        VDP_drawText( "Press -B- to continue                  ", 1, 27 );
    }

    if ( state == 5 ) {
        VDP_drawText( "S&H: ON                                ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON:  ON, RUINS:  ON      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS:  ON, DEATH:  ON   ", 1, 2 );
        VDP_drawText( "Everything has priority, so everything ", 1, 25 );
        VDP_drawText( "renders at normal color and order.     ", 1, 26 );
        VDP_drawText( "Press -C- to continue                  ", 1, 27 );
    }

    if ( state == 6 ) {
        VDP_drawText( "S&H: ON                                ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON: OFF, RUINS: OFF      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS: OFF, DEATH: OFF   ", 1, 2 );
        VDP_drawText( "Death painted entirely with color 13   ", 1, 25 );
        VDP_drawText( "of PAL3 — appears dark without priority", 1, 26 );
        VDP_drawText( "Press -A-                              ", 1, 27 );
    }

    if ( state == 7 ) {
        VDP_drawText( "S&H: ON                                ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON: OFF, RUINS: OFF      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS: OFF, DEATH:  ON   ", 1, 2 );
        VDP_drawText( "With priority, color 13 renders normal.", 1, 25 );
        VDP_drawText( "Now if we paint Death with color 14 of ", 1, 26 );
        VDP_drawText( "PAL3 (4th palette) ...   Press -B-     ", 1, 27 );
    }

    if ( state == 8 ) {
        VDP_drawText( "S&H: ON                                ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON:  ON, RUINS:  ON      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS: OFF, DEATH:  ON   ", 1, 2 );
        VDP_drawText( "Death with color 14 and S/H active     ", 1, 25 );
        VDP_drawText( "appears bright and transparent         ", 1, 26 );
        VDP_drawText( "Press -C- to continue                  ", 1, 27 );
    }

    if ( state == 9 ) {
        VDP_drawText( "S&H: ON                                ", 1, 0 );
        VDP_drawText( "BG PRIORITY MOON:  ON, RUINS:  ON      ", 1, 1 );
        VDP_drawText( "SPR PRIORITY MORRIS: OFF, DEATH:  ON   ", 1, 2 );
        VDP_drawText( "Death with color 15 and S/H active     ", 1, 25 );
        VDP_drawText( "appears dark and transparent           ", 1, 26 );
        VDP_drawText( "Press -A- to continue                  ", 1, 27 );
    }

    if ( state == 10 ) {
        VDP_drawText( "    PRIORITY MAP EXAMPLE               ", 1, 0 );
        VDP_drawText( "      Press -B- to continue            ", 1, 1 );
    }

    if ( state == 11 ) {
        VDP_drawText( "    PRIORITY MAP EXAMPLE (2)           ", 1, 0 );
        VDP_drawText( "    Redraws background line by line    ", 1, 1 );
        VDP_drawText( "      Press -C- to continue            ", 1, 2 );
    }

    if ( state == 12 ) {
        VDP_drawText( "    PRIORITY MAP EXAMPLE (3)           ", 1, 0 );
        VDP_drawText( "    Redraws background line by line    ", 1, 1 );
        VDP_drawText( "         dynamically                   ", 1, 2 );
        VDP_drawText( "      UP/DOWN: Change the diagonal     ", 1, 3 );
    }

}

// moves Death across the screen with a sinusoidal vertical movement
static void moveEnemies( void ) {

    // horizontal: scrolls left; wraps around when it exits the screen
    deathX -= moveSpeed;
    // OPTIONAL horizontal bounce:
    // deathX -= moveSpeed * enemyDirection;
    // if ( deathX >= 320 || deathX <= -40 ) { enemyDirection *= -1; }

    // vertical: sinusoidal oscillation around DEATH_Y_MIDPOINT
    deathY = DEATH_Y_MIDPOINT + sinFix16( deathX * sineSpeed ) * amplitude;

    // clamp to screen bounds
    if ( deathY <= 0 )   { deathY = 0; }
    if ( deathY >= 224 ) { deathY = 224; }

    SPR_setPosition( deathSprite, deathX, deathY );

}
