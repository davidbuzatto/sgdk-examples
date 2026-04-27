/**
 * @file main.c
 * @author Daniel Bustos "danibus" (based on original by RealBrucest)
 * @brief Lesson 08 - "Shadow and Highlight (4) - SF2 with Priority Maps" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

#include "backgrounds.h"
#include "sprite.h"

// sprite animations
#define ANIM_STAND  0
#define ANIM_WALK   1

// screen dimensions in tiles (one plane = 40x28 tiles)
#define SCREEN_WIDTH_TILES    40
#define SCREEN_HEIGHT_TILES   28

#define SCENARIO_POS_X        0
#define SCENARIO_POS_Y        0

#define SCENARIO_WIDTH_TILES  SCREEN_WIDTH_TILES
#define SCENARIO_HEIGHT_TILES SCREEN_HEIGHT_TILES

// total tiles in one screen = 40 x 28 = 1120
#define SCENARIO_NUM_TILES    ( SCENARIO_WIDTH_TILES * SCENARIO_HEIGHT_TILES )

// function declarations
static void handleInput( void );

// sprites
Sprite *ryuSprite;
Sprite *shadowSprite;
Sprite *hadokenSprite;

// initial sprite positions
u32 ryuPosx      = 120;
u32 ryuPosy      = 120;
u32 hadokenPosx  = 150;
u32 hadokenPosy  = 130;

int main( bool hard ) {

    // SETUP ---------------------------------------------------------------

    // tilemap buffer for plane A — one entry per screen tile
    u16 tilemapBuffer[SCENARIO_NUM_TILES];

    // pointer to the first buffer entry
    u16 *planeTilemap = &tilemapBuffer[0];

    // zero-initialise the buffer (default content is undefined)
    for ( int j = 0; j < SCENARIO_NUM_TILES; j++ ) {
        tilemapBuffer[j] = 0;
    }

    // pointer to the priority map image tilemap
    TileMap *shadowTilemap = bg_priority.tilemap;

    // start from the last tile and work backwards
    u16 numTiles = SCENARIO_NUM_TILES;

    // PRIORITY PASS -------------------------------------------------------
    // Walk every tile in the screen. For each tile, check the corresponding
    // entry in the priority map image. If that entry is non-zero (any color
    // other than palette index 0 / black), mark the plane A tile with high
    // priority (TILE_ATTR_PRIORITY_MASK). Zero entries are left as-is.
    while ( numTiles-- ) {

        if ( shadowTilemap ) {
            // equivalent to: *planeTilemap = *planeTilemap | TILE_ATTR_PRIORITY_MASK
            // sets only the priority bit; all other tile attributes are preserved
            *planeTilemap |= TILE_ATTR_PRIORITY_MASK;
        }

        planeTilemap++;  // advance to the next plane A tile
        shadowTilemap++; // advance to the next priority-map tile

    }

    // At this point tilemapBuffer holds all 1120 tile entries with their
    // priority bits correctly set based on the priority map image.

    // upload the priority tilemap to plane A at position (0, 0), 40x28 tiles
    VDP_setTileMapDataRectEx(
        BG_A, &tilemapBuffer[0], 0,
        SCENARIO_POS_X, SCENARIO_POS_Y,
        SCENARIO_WIDTH_TILES, SCENARIO_HEIGHT_TILES, SCENARIO_WIDTH_TILES, CPU
    );

    // load plane B with the real background image as-is
    VDP_drawImage( BG_B, &bg_B_real, SCENARIO_POS_X, SCENARIO_POS_Y );

    // SPRITES -------------------------------------------------------------

    // SPR_init( 16, 256, 256 );
    // SPR_init();
    SPR_initEx( 528 );

    // palette from the background image
    PAL_setPalette( PAL0, (u16 *) bg_B_real.palette->data, CPU );

    // palettes from the sprite sheets
    PAL_setPalette( PAL1, (u16 *) ryu_sprite.palette->data, CPU );
    PAL_setPalette( PAL3, (u16 *) hadoken_sprite.palette->data, CPU );

    ryuSprite     = SPR_addSprite( &ryu_sprite,     ryuPosx,      ryuPosy,          TILE_ATTR( PAL1, FALSE, FALSE, FALSE ) );
    shadowSprite  = SPR_addSprite( &shadow_sprite,  ryuPosx,      ryuPosy + 70,     TILE_ATTR( PAL3, FALSE, FALSE, FALSE ) );
    hadokenSprite = SPR_addSprite( &hadoken_sprite,  hadokenPosx,  hadokenPosy,     TILE_ATTR( PAL3, TRUE,  FALSE, FALSE ) );

    // enable shadow/highlight mode
    VDP_setHilightShadow( 1 );

    SPR_setHFlip( ryuSprite, TRUE );
    SPR_setHFlip( hadokenSprite, TRUE );
    SPR_update();

    while ( TRUE ) {

        handleInput();

        SPR_update();

        VDP_waitVSync();

    }

    return 0;

}

// reads joypad and updates sprite positions and animations
static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        SPR_setPosition( ryuSprite,    ryuPosx--,  ryuPosy );
        SPR_setPosition( shadowSprite, ryuPosx--,  ryuPosy + 70 );
        SPR_setAnim( ryuSprite, ANIM_WALK );
        SPR_setHFlip( ryuSprite, FALSE );
    }

    if ( value & BUTTON_RIGHT ) {
        SPR_setPosition( ryuSprite,    ryuPosx++,  ryuPosy );
        SPR_setPosition( shadowSprite, ryuPosx++,  ryuPosy + 70 );
        SPR_setAnim( ryuSprite, ANIM_WALK );
        SPR_setHFlip( ryuSprite, TRUE );
    }

    if ( value & BUTTON_UP ) {
        SPR_setPosition( ryuSprite,    ryuPosx, ryuPosy-- );
        SPR_setPosition( shadowSprite, ryuPosx, ryuPosy + 69 );
    }

    if ( value & BUTTON_DOWN ) {
        SPR_setPosition( ryuSprite,    ryuPosx, ryuPosy++ );
        SPR_setPosition( shadowSprite, ryuPosx, ryuPosy + 69 );
    }

    if ( value & BUTTON_A ) {
        SPR_setPosition( hadokenSprite, hadokenPosx++, hadokenPosy );
    }

    if ( value & BUTTON_B ) {
        SPR_setPosition( hadokenSprite, hadokenPosx--, hadokenPosy );
    }

    // no horizontal input — return to stand animation
    if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) ) {
        SPR_setAnim( ryuSprite, ANIM_STAND );
    }

}
