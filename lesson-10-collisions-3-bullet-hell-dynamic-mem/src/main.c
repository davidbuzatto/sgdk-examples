/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 10 - "Collisions (3) - Bullet Hell (Dynamic Memory)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include "sprites.h"
#include "fondos.h"

// ship sprite animation indices
#define SHIP_IDLE             0
#define SHIP_UP               1
#define SHIP_DOWN             2

#define PLAYER_SPEED          2

// fire lockout: cycles to wait between shots (lower = faster fire rate)
#define FIRE_LOCKOUT          10
#define BOSS_FIRE_LOCKOUT      5

#define MAX_PLAYER_BULLETS    15
#define MAX_BOSS_BULLETS      40
#define MAX_EXPLOSIONS        10

#define PLAYER_BULLET_SPEED    6
#define BOSS_BULLET_SPEED      1

// player hitbox (coordinates relative to ship position)
#define PLAYER_BOX_X1          8
#define PLAYER_BOX_Y1          8
#define PLAYER_BOX_X2         16
#define PLAYER_BOX_Y2         16

// boss hitbox (absolute screen coordinates — boss is a background layer)
#define BOSS_BOX_X1          230
#define BOSS_BOX_Y1           35
#define BOSS_BOX_X2          290
#define BOSS_BOX_Y2          145

#define EXPLOSION_LIFETIME    10

// function declarations
static void initPlayerBullets( void );
static void initExplosions( void );
static void initBossBullets( void );

static void spawnPlayerBullet( int type );
static void updatePlayerBullets( void );

static void spawnExplosion( int x, int y );
static void updateExplosions( void );

static void spawnBossBullet( void );
static void updateBossBullets( void );

static void updatePlayerCollisionBox( void );

static void handleInput( void );
static void showDebug( void );

// player ship
struct {
    Sprite *sprite;
    int x, y;
    int x1, y1, x2, y2; // bounding box
    int fireTimer;
} ship;

// boss (rendered as a background; no sprite pointer needed)
struct {
    int x1, y1, x2, y2;
    int fireTimer;
} boss;

// player bullet type (static array)
struct Bullet {
    int     active, x, y;
    int     type;    // 0=straight, 1=diagonal-up, 2=diagonal-down
    Sprite *sprite;
};

struct Bullet playerBullets[MAX_PLAYER_BULLETS];

// boss bullet type (heap-allocated array)
typedef struct {
    int     active, x, y;
    int     type;
    Sprite *sprite;
} BossBullet;

BossBullet *bossBullets;
int         numBossBullets;

// explosion type (static array)
struct Explosion {
    int     active, x, y;
    int     lifetime;
    Sprite *sprite;
};

struct Explosion explosions[MAX_EXPLOSIONS];


static void initPlayerBullets( void ) {

    for ( int i = 0; i < MAX_PLAYER_BULLETS; i++ ) {
        playerBullets[i].active = 0;
        playerBullets[i].x      = 0;
        playerBullets[i].y      = 0;
        playerBullets[i].type   = 0;
        playerBullets[i].sprite = NULL;
    }

}

static void initExplosions( void ) {

    for ( int i = 0; i < MAX_EXPLOSIONS; i++ ) {
        explosions[i].x      = 0;
        explosions[i].y      = 0;
        explosions[i].sprite = NULL;
    }

}

static void initBossBullets( void ) {

    bossBullets = MEM_alloc( MAX_BOSS_BULLETS * sizeof( BossBullet ) );

    for ( int i = 0; i < MAX_BOSS_BULLETS; i++ ) {
        bossBullets[i].active = 0;
        bossBullets[i].x      = 0;
        bossBullets[i].y      = 0;
        bossBullets[i].type   = 0;
        bossBullets[i].sprite = NULL;
    }

    numBossBullets = 0;

}


// spawns a player bullet in the first free slot; type: 0=straight, 1/2=diagonal
static void spawnPlayerBullet( int type ) {

    for ( int i = 0; i < MAX_PLAYER_BULLETS; i++ ) {

        if ( playerBullets[i].active == 0 ) {
            playerBullets[i].active = 1;
            playerBullets[i].x      = ship.x + 30;
            playerBullets[i].y      = ship.y + 15;
            playerBullets[i].type   = type;
            playerBullets[i].sprite = SPR_addSprite(
                &bala_sprite,
                playerBullets[i].x,
                playerBullets[i].y,
                TILE_ATTR( PAL0, TRUE, FALSE, FALSE )
            );
            break;
        }

    }

}


// advances all active player bullets; removes them when off-screen or hitting the boss
static void updatePlayerBullets( void ) {

    for ( int i = 0; i < MAX_PLAYER_BULLETS; i++ ) {

        if ( playerBullets[i].active != 0 ) {

            if ( playerBullets[i].type == 0 ) {
                playerBullets[i].x += PLAYER_BULLET_SPEED;
            } else if ( playerBullets[i].type == 1 ) {
                playerBullets[i].x += PLAYER_BULLET_SPEED;
                playerBullets[i].y--;
            } else if ( playerBullets[i].type == 2 ) {
                playerBullets[i].x += PLAYER_BULLET_SPEED;
                playerBullets[i].y++;
            }

            SPR_setPosition( playerBullets[i].sprite, playerBullets[i].x, playerBullets[i].y );

            if ( playerBullets[i].x >= 330 ||
                 playerBullets[i].y <= 0   ||
                 playerBullets[i].y >= 220 ) {
                playerBullets[i].active = 0;
                SPR_releaseSprite( playerBullets[i].sprite );
            }

            if ( playerBullets[i].x > boss.x1 && playerBullets[i].x < boss.x2 &&
                 playerBullets[i].y > boss.y1 && playerBullets[i].y < boss.y2 ) {
                playerBullets[i].active = 0;
                SPR_releaseSprite( playerBullets[i].sprite );
                spawnExplosion( playerBullets[i].x, playerBullets[i].y );
            }

        }

    }

}


// spawns an explosion at (x, y) in the first free slot
static void spawnExplosion( int x, int y ) {

    for ( int i = 0; i < MAX_EXPLOSIONS; i++ ) {

        if ( explosions[i].active == 0 ) {
            explosions[i].active   = 1;
            explosions[i].x        = x;
            explosions[i].y        = y;
            explosions[i].lifetime = EXPLOSION_LIFETIME;
            explosions[i].sprite   = SPR_addSprite(
                &explosion_sprite,
                explosions[i].x,
                explosions[i].y,
                TILE_ATTR( PAL0, TRUE, FALSE, FALSE )
            );
            break;
        }

    }

}


// counts down each explosion's lifetime; removes it when it reaches zero
static void updateExplosions( void ) {

    for ( int i = 0; i < MAX_EXPLOSIONS; i++ ) {

        if ( explosions[i].active != 0 ) {
            explosions[i].lifetime--;
            if ( explosions[i].lifetime <= 0 ) {
                explosions[i].active = 0;
                SPR_releaseSprite( explosions[i].sprite );
            }
        }

    }

}


// spawns a boss bullet in the first free slot at a random vertical position
static void spawnBossBullet( void ) {

    for ( int i = 0; i < MAX_BOSS_BULLETS; i++ ) {

        if ( bossBullets[i].active == 0 ) {
            bossBullets[i].active = 1;
            bossBullets[i].type   = ( ( ( random() % 3 ) - 1 ) + 1 ); // yields 0, 1, or 2
            bossBullets[i].x      = 220;
            bossBullets[i].y      = ( ( ( random() % 200 ) - 1 ) + 1 );
            bossBullets[i].sprite = SPR_addSprite(
                &bala_sprite,
                bossBullets[i].x,
                bossBullets[i].y,
                TILE_ATTR( PAL0, TRUE, FALSE, FALSE )
            );
            numBossBullets++;
            break;
        }

    }

}


// advances all active boss bullets; exits the loop early once all active ones are processed
static void updateBossBullets( void ) {

    for ( int i = 0, processed = 0; i < MAX_BOSS_BULLETS; i++ ) {

        if ( bossBullets[i].active == 1 ) {

            processed++;
            if ( processed > numBossBullets ) { break; }

            if ( bossBullets[i].type == 0 ) {
                bossBullets[i].x -= BOSS_BULLET_SPEED;
            }
            if ( bossBullets[i].type == 1 ) {
                bossBullets[i].x -= BOSS_BULLET_SPEED;
                bossBullets[i].y--;
            }
            if ( bossBullets[i].type == 2 ) {
                bossBullets[i].x -= BOSS_BULLET_SPEED;
                bossBullets[i].y++;
            }

            SPR_setPosition( bossBullets[i].sprite, bossBullets[i].x, bossBullets[i].y );

            if ( bossBullets[i].x <= 0 ||
                 bossBullets[i].y <= 0 ||
                 bossBullets[i].y >= 220 ) {
                bossBullets[i].active = 0;
                SPR_releaseSprite( bossBullets[i].sprite );
                numBossBullets--;
            } else if ( bossBullets[i].x > ship.x1 && bossBullets[i].x < ship.x2 &&
                        bossBullets[i].y > ship.y1 && bossBullets[i].y < ship.y2 ) {
                bossBullets[i].active = 0;
                SPR_releaseSprite( bossBullets[i].sprite );
                spawnExplosion( bossBullets[i].x, bossBullets[i].y );
                numBossBullets--;
            }

        }

    }

}


static void updatePlayerCollisionBox( void ) {

    ship.x1 = ship.x + PLAYER_BOX_X1;
    ship.y1 = ship.y + PLAYER_BOX_Y1;
    ship.x2 = ship.x + PLAYER_BOX_X2;
    ship.y2 = ship.y + PLAYER_BOX_Y2;

}


static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT )  { SPR_setPosition( ship.sprite, ship.x -= PLAYER_SPEED, ship.y ); }
    if ( value & BUTTON_RIGHT ) { SPR_setPosition( ship.sprite, ship.x += PLAYER_SPEED, ship.y ); }

    if ( value & BUTTON_UP ) {
        SPR_setPosition( ship.sprite, ship.x, ship.y -= PLAYER_SPEED );
        SPR_setAnim( ship.sprite, SHIP_UP );
    }

    if ( value & BUTTON_DOWN ) {
        SPR_setPosition( ship.sprite, ship.x, ship.y += PLAYER_SPEED );
        SPR_setAnim( ship.sprite, SHIP_DOWN );
    }

    if ( !( value & BUTTON_UP ) && !( value & BUTTON_DOWN ) ) {
        SPR_setAnim( ship.sprite, SHIP_IDLE );
    }

    if ( ship.x <= 0 )        { ship.x =   2; }
    if ( ship.x >= 320 - 32 ) { ship.x = 320 - 32 - 2; }
    if ( ship.y <= 0 )        { ship.y =   2; }
    if ( ship.y >= 224 - 24 ) { ship.y = 224 - 24 - 2; }

    if ( ( value & BUTTON_A ) && ship.fireTimer == 0 ) {
        spawnPlayerBullet( 0 );
        ship.fireTimer = 1;
    }

    if ( ship.fireTimer > 0 ) { ship.fireTimer++; }
    if ( ship.fireTimer > FIRE_LOCKOUT ) { ship.fireTimer = 0; }

    if ( ( value & BUTTON_B ) && ship.fireTimer == 0 ) {
        spawnPlayerBullet( 0 );
        spawnPlayerBullet( 1 );
        spawnPlayerBullet( 2 );
        ship.fireTimer = 1;
    }

    if ( ( value & BUTTON_C ) && ( boss.fireTimer == 0 || boss.fireTimer == 1 ) ) {
        spawnBossBullet();
        boss.fireTimer++;
    }

    if ( boss.fireTimer > 0 ) { boss.fireTimer++; }
    if ( boss.fireTimer > BOSS_FIRE_LOCKOUT ) { boss.fireTimer = 0; }

    if ( value & BUTTON_START ) {
        MEM_free( bossBullets );
    }

}


static void showDebug( void ) {

    char buf[32];
    int bulletCount     = 0;
    int bossBulletCount = 0;
    int explosionCount  = 0;

    for ( int i = 0; i < MAX_PLAYER_BULLETS; i++ ) {
        if ( playerBullets[i].active != 0 ) { bulletCount++; }
    }

    for ( int i = 0; i < MAX_BOSS_BULLETS; i++ ) {
        if ( bossBullets[i].active != 0 ) { bossBulletCount++; }
    }

    for ( int i = 0; i < MAX_EXPLOSIONS; i++ ) {
        if ( explosions[i].active != 0 ) { explosionCount++; }
    }

    sprintf( buf, "%4d", bulletCount );
    VDP_drawText( "Ship Bullets:", 1, 21 );
    VDP_drawText( buf, 15, 21 );

    sprintf( buf, "%4d", bossBulletCount );
    VDP_drawText( "Boss Bullets:", 1, 22 );
    VDP_drawText( buf, 15, 22 );

    sprintf( buf, "%4d", explosionCount );
    VDP_drawText( "Explosions:  ", 1, 23 );
    VDP_drawText( buf, 15, 23 );

    sprintf( buf, "%4d", bulletCount + bossBulletCount + explosionCount );
    VDP_drawText( "Total sprites:", 1, 24 );
    VDP_drawText( buf, 15, 24 );

    VDP_showFPS( FALSE, 1, 1 );

}


int main( bool hard ) {

    initPlayerBullets();
    initExplosions();
    initBossBullets();

    ship.x         = 64;
    ship.y         = 145;
    ship.fireTimer = 0;

    boss.fireTimer = 0;
    boss.x1        = BOSS_BOX_X1;
    boss.y1        = BOSS_BOX_Y1;
    boss.x2        = BOSS_BOX_X2;
    boss.y2        = BOSS_BOX_Y2;

    u16 ind;

    VDP_setScreenWidth320();

    SPR_init();

    PAL_setPalette( PAL0, nave_sprite.palette->data, CPU );

    ind = TILE_USER_INDEX;
    VDP_drawImageEx( BG_B, &fondo2, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += fondo2.tileset->numTile;
    VDP_drawImageEx( BG_A, &fondo1, TILE_ATTR_FULL( PAL0, TRUE,  FALSE, FALSE, ind ), 0, 0, FALSE, TRUE );
    ind += fondo1.tileset->numTile;

    VDP_setScrollingMode( HSCROLL_PLANE, VSCROLL_PLANE );
    int scrollOffset = 0;

    ship.sprite = SPR_addSprite( &nave_sprite, ship.x, ship.y, TILE_ATTR( PAL0, TRUE, FALSE, FALSE ) );

    VDP_drawText( "      BULLET HELL HELP    ", 1, 1 );
    VDP_drawText( "PAD - Controls, A/B: Fire ", 2, 2 );
    VDP_drawText( "C: Boss Bullet Hell       ", 2, 3 );

    while ( TRUE ) {

        showDebug();

        handleInput();

        updatePlayerCollisionBox();

        updatePlayerBullets();
        updateBossBullets();
        updateExplosions();

        scrollOffset -= 3;
        VDP_setHorizontalScroll( BG_B, scrollOffset );

        SPR_update();
        SYS_doVBlankProcess();

    }

    return 0;

}
