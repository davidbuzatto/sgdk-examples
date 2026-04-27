/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 02 - "Controls" (asynchronous mode) example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

// the asynchronous callback fires whenever a button state changes
static void joyEvent( u16 joy, u16 changed, u16 state );

int main( bool hard ) {

    VDP_drawText( "Controls: ASYNCHRONOUS MODE", 6, 4 );

    // register the input handler before the main loop
    JOY_setEventHandler( joyEvent );

    while ( TRUE ) {

        // nothing needed here — the callback handles all input events
        SYS_doVBlankProcess();

    }

    return 0;

}

static void joyEvent( u16 joy, u16 changed, u16 state ) {

    if ( state & BUTTON_LEFT ) {
        VDP_drawText( "LEFT", 8, 10 );
    }

    if ( state & BUTTON_RIGHT ) {
        VDP_drawText( "RIGHT", 20, 10 );
    }

    if ( state & BUTTON_UP ) {
        VDP_drawText( "UP", 16, 8 );
    }

    if ( state & BUTTON_DOWN ) {
        VDP_drawText( "DOWN", 16, 12 );
    }

    if ( state & BUTTON_A ) {
        VDP_drawText( "BUTTON A", 6, 18 );
    }

    if ( state & BUTTON_B ) {
        VDP_drawText( "BUTTON B", 14, 18 );
    }

    if ( state & BUTTON_C ) {
        VDP_drawText( "BUTTON C", 22, 18 );
    }

    if ( state & BUTTON_X ) {
        VDP_drawText( "BUTTON X", 6, 20 );
    }

    if ( state & BUTTON_Y ) {
        VDP_drawText( "BUTTON Y", 14, 20 );
    }

    if ( state & BUTTON_Z ) {
        VDP_drawText( "BUTTON Z", 22, 20 );
    }

    if ( state & BUTTON_START ) {
        VDP_drawText( "START", 10, 16 );
    }

    if ( state & BUTTON_MODE ) {
        VDP_drawText( "MODE", 20, 16 );
    }

    // when a button is released, clear the area below the title
    // VDP_clearTextArea() works in tiles
    if ( ( !( state & BUTTON_LEFT )  && ( changed & BUTTON_LEFT ) )   ||
         ( !( state & BUTTON_RIGHT ) && ( changed & BUTTON_RIGHT ) )  ||
         ( !( state & BUTTON_UP )    && ( changed & BUTTON_UP ) )     ||
         ( !( state & BUTTON_DOWN )  && ( changed & BUTTON_DOWN ) )   ||
         ( !( state & BUTTON_A )     && ( changed & BUTTON_A ) )      ||
         ( !( state & BUTTON_B )     && ( changed & BUTTON_B ) )      ||
         ( !( state & BUTTON_C )     && ( changed & BUTTON_C ) )      ||
         ( !( state & BUTTON_X )     && ( changed & BUTTON_X ) )      ||
         ( !( state & BUTTON_Y )     && ( changed & BUTTON_Y ) )      ||
         ( !( state & BUTTON_Z )     && ( changed & BUTTON_Z ) )      ||
         ( !( state & BUTTON_START ) && ( changed & BUTTON_START ) )  ||
         ( !( state & BUTTON_MODE )  && ( changed & BUTTON_MODE ) ) ) {
        VDP_clearTextArea( 0, 7, 40, 28 );
    }

}
