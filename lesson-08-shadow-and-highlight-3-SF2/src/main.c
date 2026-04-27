/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 08 - "Shadow and Highlight (3) - Street Fighter II" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include "fondos.h"
#include "sprite.h"

#define ANIM_STAND  0
#define ANIM_WALK   1

// function declarations
static void handleInput( void );

// sprites
Sprite *ryuSprite;
Sprite *shadowSprite;
Sprite *hadokenSprite;

// initial sprite positions on screen
u32 posx           = 120;
u32 posy           = 120;
u32 hadokenPosx    = 150;
u32 hadokenPosy    = 130;

// hadoken starts with high priority
bool hadokenPriority = TRUE;

int main( bool hard ) {

    // tile counter in VRAM
    u16 ind;

    // set display resolution to 320x224
    VDP_setScreenWidth320();

    // initialize the sprite engine
    SPR_init();

    // load background palettes
    PAL_setPalette( PAL0, fondo1.palette->data, CPU );
    PAL_setPalette( PAL1, fondo2.palette->data, CPU );

    // load both backgrounds into the VDP
    ind = TILE_USER_INDEX;
    VDP_drawImageEx( BG_A, &fondo1, TILE_ATTR_FULL( PAL0, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += fondo1.tileset->numTile;
    VDP_drawImageEx( BG_B, &fondo2, TILE_ATTR_FULL( PAL1, TRUE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += fondo2.tileset->numTile;

    // load sprite palettes
    PAL_setPalette( PAL2, mi_sprite_ryu.palette->data, CPU );
    PAL_setPalette( PAL3, mi_sprite_sombra.palette->data, CPU );

    // add sprites
    ryuSprite    = SPR_addSprite( &mi_sprite_ryu,    posx,          posy,          TILE_ATTR( PAL2, TRUE,               FALSE, FALSE ) );
    shadowSprite = SPR_addSprite( &mi_sprite_sombra, posx,          posy + 70,     TILE_ATTR( PAL3, TRUE,               FALSE, FALSE ) );
    hadokenSprite= SPR_addSprite( &mi_sprite_hadoken, hadokenPosx,  hadokenPosy,   TILE_ATTR( PAL3, hadokenPriority,    FALSE, FALSE ) );

    // enable shadow/highlight mode
    VDP_setHilightShadow( 1 );

    SPR_setHFlip( ryuSprite, TRUE );
    SPR_setHFlip( hadokenSprite, TRUE );

    // on-screen help (text uses PAL3 so it renders in the shadow layer)
    VDP_setTextPalette( PAL3 );
    VDP_drawText( "SHADOW & HIGHLIGHT : SF2                ", 2, 1 );
    VDP_drawText( "D-PAD: MOVE RYU, A/B: MOVE HA-DO-KEN   ", 2, 2 );

    SPR_update();

    while ( TRUE ) {

        handleInput();

        SPR_update();

        SYS_doVBlankProcess();

    }

    return 0;

}

// reads joypad and updates sprite positions and animations
static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        SPR_setPosition( ryuSprite,    posx--,        posy );
        SPR_setPosition( shadowSprite, posx--,        posy + 70 );
        SPR_setAnim( ryuSprite, ANIM_WALK );
        SPR_setHFlip( ryuSprite, FALSE );
    }

    if ( value & BUTTON_RIGHT ) {
        SPR_setPosition( ryuSprite,    posx++,        posy );
        SPR_setPosition( shadowSprite, posx++,        posy + 70 );
        SPR_setAnim( ryuSprite, ANIM_WALK );
        SPR_setHFlip( ryuSprite, TRUE );
    }

    if ( value & BUTTON_UP ) {
        SPR_setPosition( ryuSprite, posx, posy-- );
    }

    if ( value & BUTTON_DOWN ) {
        SPR_setPosition( ryuSprite, posx, posy++ );
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
