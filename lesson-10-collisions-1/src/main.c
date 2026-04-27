/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 10 - "Collisions (1)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include "sprites.h"

// collision tolerance radius (pixels)
#define TOLERANCE   10

// Sonic still-frame indices within animation 0
#define SONIC_IDLE  0
#define SONIC_IDLE2 1
#define SONIC_IDLE3 2

// maximum simultaneous bullets on screen (part 3)
#define MAX_BULLETS 10

// function declarations — part 1
static void handleInputPart1( void );
static void drawPositions( void );
void hblankCallback( void );

// function declarations — part 2
static void checkCollision1( void );
static void checkCollision2( void );
static void handleInputPart2( void );

// function declarations — part 3
static void checkCollision3( void );
static void checkCollision4( void );
static void handleInputPart3( void );

// part 1: two bullet sprites
Sprite *bulletSprite;
Sprite *bulletSprite2;

int bulletPosx  = 64;
int bulletPosy  = 145;
int bullet2Posx = 100;
int bullet2Posy = 100;

// 0: HW collision flag,  1: distance-based collision
int collisionType = 0;

// part 2: Sonic sprite
Sprite *sonicSprite;

int sonicPosx = 200;
int sonicPosy =  85;

// axis-aligned bounding box used for box collision checks
struct {
    int x1, y1, x2, y2;
} collisionBox;

// part 3: bullet array
Sprite *bullets[MAX_BULLETS];
fix32   bulletsPosx[MAX_BULLETS];
fix32   bulletsPosy[MAX_BULLETS];


int main( bool hard ) {

    /****************** PART 1: HW COLLISION AND DISTANCE COLLISION ******************/

    VDP_setScreenWidth320();

    SPR_init();

    PAL_setPalette( PAL1, bullet_sprite.palette->data, CPU );

    bulletSprite  = SPR_addSprite( &bullet_sprite, bulletPosx,  bulletPosy,  TILE_ATTR( PAL1, TRUE, FALSE, FALSE ) );
    bulletSprite2 = SPR_addSprite( &bullet_sprite, bullet2Posx, bullet2Posx, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ) );

    VDP_drawText( "HW AND DISTANCE COLLISION        ", 2, 3  );
    VDP_drawText( "A - HW VDP collision bit         ", 2, 4  );
    VDP_drawText( "B - Distance-based collision     ", 2, 5  );
    VDP_drawText( "Press - START - to continue      ", 2, 26 );

    SYS_setHIntCallback( hblankCallback );
    VDP_setHIntCounter( 8 );
    VDP_setHInterrupt( 1 );

    while ( TRUE ) {

        handleInputPart1();

        u16 value = JOY_readJoypad( JOY_1 );
        if ( value & BUTTON_START ) { break; }

        if ( collisionType == 0 ) {
            VDP_drawText( "HW collision bit active?     ", 2, 20 );
            if ( GET_VDP_STATUS( VDP_SPRCOLLISION_FLAG ) != 0 ) {
                VDP_drawText( "Yes", 26, 20 );
            } else {
                VDP_drawText( "No ", 26, 20 );
            }
        }

        if ( collisionType == 1 ) {
            VDP_drawText( "Within collision range?      ", 2, 20 );
            if ( abs( bulletPosx - bullet2Posx ) < TOLERANCE &&
                 abs( bulletPosy - bullet2Posy ) < TOLERANCE ) {
                VDP_drawText( "Yes", 26, 20 );
            } else {
                VDP_drawText( "No ", 26, 20 );
            }
        }

        drawPositions();

        SPR_update();
        VDP_waitVSync();

    }


    /****************** PART 2: BOUNDING BOX COLLISION ******************/

    SYS_setHIntCallback( NULL );
    VDP_setHIntCounter( NULL );
    VDP_setHInterrupt( NULL );

    SPR_reset();
    SPR_init();

    VDP_clearTextAreaBG( BG_A, 0, 20, 40, 28 );

    bulletPosx = 100;
    bulletPosy = 100;
    bulletSprite = SPR_addSprite( &bullet_sprite, bulletPosx, bulletPosy, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ) );

    PAL_setPalette( PAL2, sonic_sprite.palette->data, CPU );

    sonicSprite = SPR_addSprite( &sonic_sprite, sonicPosx, sonicPosy, TILE_ATTR( PAL2, TRUE, FALSE, FALSE ) );
    SPR_setAnimAndFrame( sonicSprite, 0, SONIC_IDLE2 );

    collisionType = 1;

    VDP_drawText( "BOUNDING BOX COLLISION           ", 2, 3  );
    VDP_drawText( "A - Origin-point collision       ", 2, 4  );
    VDP_drawText( "B - Bounding box collision       ", 2, 5  );
    VDP_drawText( "Press - C - to continue          ", 2, 26 );

    while ( TRUE ) {

        handleInputPart2();

        u16 value = JOY_readJoypad( JOY_1 );
        if ( value & BUTTON_C ) { break; }

        if ( collisionType == 1 ) { checkCollision1(); }
        if ( collisionType == 2 ) { checkCollision2(); }

        SPR_update();
        VDP_waitVSync();

    }


    /****************** PART 3: MULTI-SPRITE COLLISION ******************/

    SPR_reset();
    SPR_init();

    VDP_clearTextAreaBG( BG_A, 0, 20, 40, 28 );

    for ( int i = 0; i < MAX_BULLETS; i++ ) {
        bulletsPosx[i] = random() % 320;
        bulletsPosy[i] = random() % 224;
        bullets[i] = SPR_addSprite( &bullet_sprite, bulletsPosx[i], bulletsPosy[i], TILE_ATTR( PAL2, TRUE, FALSE, FALSE ) );
    }

    sonicPosx = 100;
    sonicPosy = 100;
    sonicSprite = SPR_addSprite( &sonic_sprite, sonicPosx, sonicPosy, TILE_ATTR( PAL2, TRUE, FALSE, FALSE ) );
    SPR_setAnimAndFrame( sonicSprite, 0, SONIC_IDLE2 );

    VDP_drawText( "MULTI-SPRITE COLLISION           ", 2, 3 );
    VDP_drawText( "A - Loop: origin-point collision ", 2, 4 );
    VDP_drawText( "B - Loop: bounding box collision ", 2, 5 );

    collisionType = 1;

    while ( TRUE ) {

        handleInputPart3();

        if ( collisionType == 1 ) { checkCollision3(); }
        if ( collisionType == 2 ) { checkCollision4(); }

        SPR_update();
        VDP_waitVSync();

    }

    return 0;

}


/********** PART 1 FUNCTIONS ****************************************/

// reads the HW sprite-collision flag inside the H-blank interrupt
// (the flag is only reliable when read from within the interrupt handler)
void hblankCallback( void ) {

    if ( GET_VDP_STATUS( VDP_SPRCOLLISION_FLAG ) != 0 ) {
        VDP_drawText( "Yes", 26, 20 );
    }

}

static void handleInputPart1( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT )  { SPR_setPosition( bulletSprite, bulletPosx--, bulletPosy ); }
    if ( value & BUTTON_RIGHT ) { SPR_setPosition( bulletSprite, bulletPosx++, bulletPosy ); }
    if ( value & BUTTON_UP )    { SPR_setPosition( bulletSprite, bulletPosx, bulletPosy-- ); }
    if ( value & BUTTON_DOWN )  { SPR_setPosition( bulletSprite, bulletPosx, bulletPosy++ ); }

    if ( value & BUTTON_A ) {
        collisionType = 0;
        SYS_setHIntCallback( hblankCallback );
        VDP_setHIntCounter( 8 );
        VDP_setHInterrupt( 1 );
    }

    if ( value & BUTTON_B ) {
        collisionType = 1;
        SYS_setHIntCallback( NULL );
        VDP_setHIntCounter( NULL );
        VDP_setHInterrupt( NULL );
    }

}

static void drawPositions( void ) {

    char buf[32];

    sprintf( buf, "%4d", bulletPosx );
    VDP_drawText( "Bullet X:", 2, 21 );
    VDP_drawText( buf, 14, 21 );

    sprintf( buf, "%4d", bulletPosy );
    VDP_drawText( "Bullet Y:", 2, 22 );
    VDP_drawText( buf, 14, 22 );

    sprintf( buf, "%4d", bullet2Posx );
    VDP_drawText( "Bullet2 X:", 2, 23 );
    VDP_drawText( buf, 14, 23 );

    sprintf( buf, "%4d", bullet2Posy );
    VDP_drawText( "Bullet2 Y:", 2, 24 );
    VDP_drawText( buf, 14, 24 );

}


/********** PART 2 FUNCTIONS ****************************************/

static void handleInputPart2( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT )  { SPR_setPosition( sonicSprite, sonicPosx--, sonicPosy ); }
    if ( value & BUTTON_RIGHT ) { SPR_setPosition( sonicSprite, sonicPosx++, sonicPosy ); }
    if ( value & BUTTON_UP )    { SPR_setPosition( sonicSprite, sonicPosx, sonicPosy-- ); }
    if ( value & BUTTON_DOWN )  { SPR_setPosition( sonicSprite, sonicPosx, sonicPosy++ ); }

    if ( value & BUTTON_A ) {
        SPR_setAnimAndFrame( sonicSprite, 0, SONIC_IDLE2 );
        collisionType = 1;
    }

    if ( value & BUTTON_B ) {
        SPR_setAnimAndFrame( sonicSprite, 0, SONIC_IDLE3 );
        collisionType = 2;
    }

}

// origin-point collision: checks whether bullet origin is within TOLERANCE of Sonic origin
static void checkCollision1( void ) {

    char buf[32];

    VDP_drawText( "Within collision range?      ", 2, 20 );

    if ( abs( sonicPosx - bulletPosx ) < TOLERANCE &&
         abs( sonicPosy - bulletPosy ) < TOLERANCE ) {
        VDP_drawText( "Yes", 26, 20 );
    } else {
        VDP_drawText( "No ", 26, 20 );
    }

    KLog_U1( "sonicPosx:", sonicPosx );
    KLog_U1( "bulletPosx:", bulletPosx );
    KLog_U1( "delta x:  ", abs( sonicPosx - bulletPosx ) );
    KLog_U1( "sonicPosy:", sonicPosy );
    KLog_U1( "bulletPosy:", bulletPosy );
    KLog_U1( "delta y:  ", abs( sonicPosy - bulletPosy ) );

    sprintf( buf, "%4d", sonicPosx );
    VDP_drawText( "Sonic X:", 2, 21 );
    VDP_drawText( buf, 16, 21 );

    sprintf( buf, "%4d", sonicPosy );
    VDP_drawText( "Sonic Y:", 2, 22 );
    VDP_drawText( buf, 16, 22 );

    sprintf( buf, "%4d", bulletPosx );
    VDP_drawText( "Bullet X:", 2, 23 );
    VDP_drawText( buf, 16, 23 );

    sprintf( buf, "%4d", bulletPosy );
    VDP_drawText( "Bullet Y:", 2, 24 );
    VDP_drawText( buf, 16, 24 );

}

// bounding box collision: checks whether the bullet origin falls inside Sonic's hitbox
static void checkCollision2( void ) {

    char buf[32];

    collisionBox.x1 = sonicPosx + 16;
    collisionBox.y1 = sonicPosy +  8;
    collisionBox.x2 = sonicPosx + 31;
    collisionBox.y2 = sonicPosy + 31;

    VDP_drawText( "Within collision range?      ", 2, 20 );

    if ( bulletPosx >= collisionBox.x1 && bulletPosx <= collisionBox.x2 &&
         bulletPosy >= collisionBox.y1 && bulletPosy <= collisionBox.y2 ) {
        VDP_drawText( "Yes", 26, 20 );
    } else {
        VDP_drawText( "No ", 26, 20 );
    }

    sprintf( buf, "%4d", sonicPosx );
    VDP_drawText( "Sonic X:", 2, 21 );
    VDP_drawText( buf, 16, 21 );

    sprintf( buf, "%4d", sonicPosy );
    VDP_drawText( "Sonic Y:", 2, 22 );
    VDP_drawText( buf, 16, 22 );

}


/********** PART 3 FUNCTIONS ****************************************/

static void handleInputPart3( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT )  { SPR_setPosition( sonicSprite, sonicPosx--, sonicPosy ); }
    if ( value & BUTTON_RIGHT ) { SPR_setPosition( sonicSprite, sonicPosx++, sonicPosy ); }
    if ( value & BUTTON_UP )    { SPR_setPosition( sonicSprite, sonicPosx, sonicPosy-- ); }
    if ( value & BUTTON_DOWN )  { SPR_setPosition( sonicSprite, sonicPosx, sonicPosy++ ); }

    if ( value & BUTTON_A ) {
        SPR_setAnimAndFrame( sonicSprite, 0, SONIC_IDLE2 );
        collisionType = 1;
    }

    if ( value & BUTTON_B ) {
        SPR_setAnimAndFrame( sonicSprite, 0, SONIC_IDLE3 );
        collisionType = 2;
    }

}

// origin-point multi-collision: counts how many bullets are within TOLERANCE of Sonic
static void checkCollision3( void ) {

    int hitCount = 0;

    VDP_drawText( "Collision detected?          ", 2, 22 );

    for ( int i = 0; i < MAX_BULLETS; i++ ) {
        if ( abs( sonicPosx - bulletsPosx[i] ) < TOLERANCE &&
             abs( sonicPosy - bulletsPosy[i] ) < TOLERANCE ) {
            hitCount++;
        }
    }

    if ( hitCount == 0 ) {
        VDP_drawText( "No ", 28, 22 );
    } else {
        VDP_drawText( "Yes", 28, 22 );
    }

}

// bounding box multi-collision: counts how many bullets fall inside Sonic's hitbox
static void checkCollision4( void ) {

    int hitCount = 0;

    collisionBox.x1 = sonicPosx + 16;
    collisionBox.y1 = sonicPosy +  8;
    collisionBox.x2 = sonicPosx + 31;
    collisionBox.y2 = sonicPosy + 31;

    VDP_drawText( "Collision detected?          ", 2, 22 );

    for ( int i = 0; i < MAX_BULLETS; i++ ) {
        if ( bulletsPosx[i] >= collisionBox.x1 && bulletsPosx[i] <= collisionBox.x2 &&
             bulletsPosy[i] >= collisionBox.y1 && bulletsPosy[i] <= collisionBox.y2 ) {
            hitCount++;
        }
    }

    if ( hitCount == 1 ) {
        VDP_drawText( "Yes", 28, 22 );
    } else {
        VDP_drawText( "No ", 28, 22 );
    }

}
