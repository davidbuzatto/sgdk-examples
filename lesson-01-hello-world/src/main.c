/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 01 - "Hello World" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>

int main( bool hard ) {

    // VDP_drawText( "text", x, y ) — (x, y) refers to TILES, not pixels (1 tile = 8 pixels)
    VDP_drawText( "Hello World!", 10, 13 );

    while ( TRUE ) {

        // read controls
        // move sprites
        // update scores
        // etc.

        SYS_doVBlankProcess(); // waits for the next Vertical Blanking start

    }

    // main() returns int, so it needs a return statement to be syntactically correct.
    // However, we should NEVER reach this point — a console game never ends.
    // If reached, an "ILLEGAL INSTRUCTION" error will occur.
    return 0;

}
