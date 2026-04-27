/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 05 - "Sprites (3) Enemies" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

#include "backgrounds.h"
#include "sprites.h"

#define ANIM_STAND  0
#define ANIM_RUN    3

// function declarations
static void handleInput( void );
static void moveEnemies( void );
static void drawPosition( void );

// Sonic sprite pointer
Sprite *sonicSprite;

// Sonic position on screen
int posx = 64;
int posy = 155;

// enemy sprite pointers and positions
Sprite *enemy[2];
u16 enemyPosx[2];
int enemyPosy[2];
int enemyDirection;

// constant for the sinusoidal movement baseline
#define SINE_BASELINE 84

int main( bool hard ) {

    // tile counter in VRAM
    u16 ind;

    // set display resolution to 320x224
    VDP_setScreenWidth320();

    // initialize the sprite engine
    SPR_init();

    // load background palettes
    PAL_setPalette( PAL0, bg1.palette->data, CPU );
    PAL_setPalette( PAL1, bg2.palette->data, CPU );

    // load background images into the VDP
    ind = TILE_USER_INDEX;

    VDP_drawImageEx( BG_B, &bg1, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bg1.tileset->numTile;

    VDP_drawImageEx( BG_A, &bg2, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += bg2.tileset->numTile;

    // load the Sonic palette into PAL2
    PAL_setPalette( PAL2, sonic_sprite.palette->data, CPU );

    // add the Sonic sprite
    sonicSprite = SPR_addSprite( &sonic_sprite, posx, posy, TILE_ATTR( PAL2, TRUE, FALSE, FALSE ) );

    // set up enemies
    enemyPosx[0] = 128;
    enemyPosy[0] = 164;
    enemyPosx[1] = 260;
    enemyPosy[1] = 84;
    enemyDirection = 1;

    // load enemy palette into PAL3
    PAL_setPalette( PAL3, enemies_sprite.palette->data, CPU );

    // add enemy sprites
    enemy[0] = SPR_addSprite( &enemies_sprite, enemyPosx[0], enemyPosy[0], TILE_ATTR( PAL3, TRUE, FALSE, FALSE ) );
    enemy[1] = SPR_addSprite( &enemies_sprite, enemyPosx[1], enemyPosy[1], TILE_ATTR( PAL3, TRUE, FALSE, FALSE ) );

    // assign the correct animation to each enemy
    SPR_setAnim( enemy[0], 1 ); // crab animation (second animation in the sheet)
    SPR_setAnim( enemy[1], 0 ); // wasp animation (first animation in the sheet)

    SPR_update(); // recommended initial flush

    while ( TRUE ) {

        // read joypad and move Sonic
        handleInput();

        // update enemy positions
        moveEnemies();

        // display the wasp's Y position on screen
        drawPosition();

        // flush sprite updates to the VDP
        SPR_update();

        // sync with the TV vertical blank
        SYS_doVBlankProcess();

    }

    return 0;

}

// reads joypad input, moves Sonic left/right, and flips the sprite accordingly
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

    // no horizontal input — return to stand animation
    if ( !( value & BUTTON_RIGHT ) && !( value & BUTTON_LEFT ) ) {
        SPR_setAnim( sonicSprite, ANIM_STAND );
    }

}

// moves both enemies each frame:
//   crab  — simple horizontal bounce between screen edges
//   wasp  — horizontal scroll with sinusoidal vertical movement
static void moveEnemies( void ) {

    int moveSpeed   = 1;
    int sineSpeed   = 1; // affects how fast the sine wave cycles (try changing it)
    int sineAmp     = 1; // affects the sine wave amplitude (try changing it)

    // CRAB (enemy[0]) — horizontal bounce
    enemyPosx[0] += moveSpeed * enemyDirection;
    if ( enemyPosx[0] >= 320 || enemyPosx[0] <= 0 ) {
        enemyDirection *= -1;
    }

    // WASP (enemy[1]) — scrolls left, wraps around; vertical position follows a sine wave
    enemyPosx[1] -= moveSpeed;
    enemyPosy[1] = SINE_BASELINE + sinFix16( enemyPosx[1] * sineSpeed ) * sineAmp;

    // push updated positions to the VDP
    SPR_setPosition( enemy[0], enemyPosx[0], enemyPosy[0] );
    SPR_setPosition( enemy[1], enemyPosx[1], enemyPosy[1] );

}

// writes the wasp's current Y position to the screen
// %3d right-aligns the value to keep the display stable when digits change
static void drawPosition( void ) {

    char buf[32];

    sprintf( buf, "Y (wasp): %3d", enemyPosy[1] );
    VDP_drawText( buf, 2, 2 );

}
