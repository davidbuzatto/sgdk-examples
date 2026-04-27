/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 02 - "Controls" (synchronous mode) example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

// function declarations
static void handleInput( void );

int main( bool hard ) {

    VDP_drawText( "Controls: SYNCHRONOUS MODE", 6, 4 );

    while ( TRUE ) {

        // read controls — called once per frame
        handleInput();

        SYS_doVBlankProcess();

    }

    return 0;

}

static void handleInput( void ) {

    // store the current joypad state
    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        VDP_drawText( "LEFT", 8, 10 );
    }

    if ( value & BUTTON_RIGHT ) {
        VDP_drawText( "RIGHT", 20, 10 );
    }

    if ( value & BUTTON_UP ) {
        VDP_drawText( "UP", 16, 8 );
    }

    if ( value & BUTTON_DOWN ) {
        VDP_drawText( "DOWN", 16, 12 );
    }

    if ( value & BUTTON_A ) {
        VDP_drawText( "BUTTON A", 6, 18 );
    }

    if ( value & BUTTON_B ) {
        VDP_drawText( "BUTTON B", 14, 18 );
    }

    if ( value & BUTTON_C ) {
        VDP_drawText( "BUTTON C", 22, 18 );
    }

    if ( value & BUTTON_X ) {
        VDP_drawText( "BUTTON X", 6, 20 );
    }

    if ( value & BUTTON_Y ) {
        VDP_drawText( "BUTTON Y", 14, 20 );
    }

    if ( value & BUTTON_Z ) {
        VDP_drawText( "BUTTON Z", 22, 20 );
    }

    if ( value & BUTTON_START ) {
        VDP_drawText( "START", 10, 16 );
    }

    if ( value & BUTTON_MODE ) {
        VDP_drawText( "MODE", 20, 16 );
    }

    // if no button is pressed, clear the area below the title
    // VDP_clearTextArea() works in tiles
    if ( !( value & BUTTON_UP )    && !( value & BUTTON_DOWN )  &&
         !( value & BUTTON_LEFT )  && !( value & BUTTON_RIGHT ) &&
         !( value & BUTTON_A )     && !( value & BUTTON_B )     &&
         !( value & BUTTON_C )     && !( value & BUTTON_X )     &&
         !( value & BUTTON_Y )     && !( value & BUTTON_Z )     &&
         !( value & BUTTON_MODE )  && !( value & BUTTON_START ) ) {
        VDP_clearTextArea( 0, 7, 40, 28 );
    }

}
