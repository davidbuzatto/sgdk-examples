/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 06 - "Debug (1)" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */

#include <genesis.h>
#include "KDebug.h" // general debugging support
#include "tools.h"  // KLog helper functions
#include "timer.h"  // timer functions

// function declarations
void reset( void );

int main( bool hard ) {

    while ( TRUE ) {

        // variable to store joypad input
        u16 value;

        // Example 1 — pause execution until ESC is pressed in the emulator
        VDP_drawText( "Lesson 6: DEBUG", 1, 1 );
        VDP_drawText( "Example 1: KDebug_Halt()               ", 1, 3 );
        VDP_drawText( "                                       ", 1, 4 );
        VDP_drawText( "   Press ESC to continue               ", 1, 5 );
        VDP_drawText( "                                       ", 1, 6 );

        KDebug_Halt();

        VDP_drawText( "                                       ", 1, 4 );
        VDP_drawText( "   You pressed ESC!                    ", 1, 5 );
        VDP_drawText( "                                       ", 1, 6 );

        VDP_drawText( "Press -A- to continue                  ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_A ) {
                break;
            }
        }

        // Example 2 — display messages in the GensKMod Messages window
        // demonstrates plain strings, variables, integers, fixed-point numbers,
        // and combinations of all the above
        VDP_drawText( "Lesson 6: DEBUG", 1, 1 );
        VDP_drawText( "Example 2: KDebug_Alert()              ", 1, 3 );
        VDP_drawText( "Displays a message in the debug window:", 1, 5 );
        VDP_drawText( "  GensKMod -> CPU/Debug/Messages.      ", 1, 6 );
        VDP_drawText( "                                       ", 1, 7 );
        VDP_drawText( "Examples:                              ", 1, 8 );
        VDP_drawText( " * Literal text and a string variable  ", 1, 9 );
        VDP_drawText( " * Blank lines                         ", 1, 10 );
        VDP_drawText( " * Positive and negative integers      ", 1, 11 );
        VDP_drawText( " * Positive and negative fixed-point   ", 1, 12 );
        VDP_drawText( " * A combination of all the above      ", 1, 13 );

        KDebug_Alert( "Hello, I am a short message" );
        KDebug_Alert( "Hello, I am an excessively long message that does not fit on a single line and therefore wraps to a second line" );
        KDebug_Alert( " " ); // blank line
        KDebug_Alert( " blank line above and below" );
        KDebug_Alert( " " ); // blank line

        char *inputStr = "this is a sample string";
        char outputStr[32];

        // print the string variable
        KDebug_Alert( inputStr );

        // print a positive integer (must be converted to text first)
        int myNumber = 1200;
        intToStr( myNumber, outputStr, 1 );
        KDebug_Alert( outputStr );

        // print a negative integer
        myNumber = -1200;
        intToStr( myNumber, outputStr, 1 );
        KDebug_Alert( outputStr );

        // print a positive fixed-point number
        fix16 myFixed = FIX16( 5.4 );
        fix16ToStr( myFixed, outputStr, 1 );
        KDebug_Alert( outputStr );

        // print a negative fixed-point number
        myFixed = FIX16( -5.4 );
        fix16ToStr( myFixed, outputStr, 1 );
        KDebug_Alert( outputStr );

        // combine strings and a number into one message
        char *strA = " ... plus six is ... ";
        char strB[50], strC[50];

        strcpy( strB, "Four..." );
        strcat( strB, strA );
        myNumber = 10;
        sprintf( strC, "%s %d", strB, myNumber );
        KDebug_Alert( strC );

        VDP_drawText( "Press -B- to continue                  ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_B ) {
                break;
            }
        }

        // Example 3 — display a variable's value in the Messages window
        // values are always shown in hexadecimal
        VDP_drawText( "Lesson 6: DEBUG                        ", 1, 1 );
        VDP_drawText( "Example 3: KDebug_AlertNumber()        ", 1, 3 );
        VDP_drawText( "Displays an integer variable           ", 1, 5 );
        VDP_drawText( "                                       ", 1, 6 );
        VDP_drawText( "int myVar =  100                       ", 1, 7 );
        VDP_drawText( "int myVar = -100                       ", 1, 8 );
        VDP_drawText( "                                       ", 1, 9 );
        VDP_drawText( "KDebug_AlertNumber shows:              ", 1, 10 );
        VDP_drawText( "  64 and FFFFFF9C respectively         ", 1, 11 );
        VDP_drawText( "  (hexadecimal representation)         ", 1, 12 );
        VDP_drawText( "                                       ", 1, 13 );
        VDP_drawText( " Not exactly intuitive!                ", 1, 14 );

        KDebug_Alert( "--- positive integer ---" );
        int myVar = 100;
        KDebug_AlertNumber( myVar );

        KDebug_Alert( "--- negative integer ---" );
        myVar = -100;
        KDebug_AlertNumber( myVar );

        VDP_drawText( "Press -C- to continue                  ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_C ) {
                break;
            }
        }

        // Example 4 — KLog functions
        // void KLog( char *text );
        // void KLog_U1( char *t1, u32 v1 );
        // void KLog_U2( char *t1, u32 v1, char *t2, u32 v2 );
        // void KLog_S1( char *t1, s32 v1 );
        // void KLog_S2( char *t1, s32 v1, char *t2, s32 v2 );
        // etc.
        VDP_drawText( "Lesson 6: DEBUG                        ", 1, 1 );
        VDP_drawText( "Example 4: KLOG() functions            ", 1, 3 );
        VDP_drawText( "               where t=text, v=integer ", 1, 4 );
        VDP_drawText( " TEXT:  KLog(t)                        ", 1, 5 );
        VDP_drawText( "                                       ", 1, 6 );
        VDP_drawText( " UNSIGNED INT: KLog_U1(t1,v1)          ", 1, 7 );
        VDP_drawText( "                                       ", 1, 8 );
        VDP_drawText( " (2 values):   KLog_U2(t1,v1,t2,v2)   ", 1, 9 );
        VDP_drawText( "                                       ", 1, 10 );
        VDP_drawText( " SIGNED INT:   KLog_S1(t1,v1)          ", 1, 11 );
        VDP_drawText( "                                       ", 1, 12 );
        VDP_drawText( " (2 values):   KLog_S2(t1,v1,t2,v2)   ", 1, 13 );
        VDP_drawText( "                                       ", 1, 14 );

        KLog( "--- KLog(t) ---" );
        char *text = "this is another sample string";
        KLog( text );

        KLog( "--- KLog_U1 (positive integer) ---" );
        int varU1 = 100;
        KLog_U1( "positive integer:", varU1 );

        KLog( "--- KLog_U2 (two positive integers) ---" );
        int varU2 = 56;
        KLog_U2( "first integer:", varU1, " ,second integer:", varU2 );

        KLog( "--- KLog_S1 (positive + negative integer) ---" );
        varU1 = +100;
        KLog_S1( "positive integer:", varU1 );

        KLog( "--- KLog_S2 (positive + negative integer) ---" );
        varU1 = +100;
        varU2 = -56;
        KLog_S2( "positive integer:", varU1, " ,negative integer:", varU2 );

        VDP_drawText( "Press -A- to continue                  ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_A ) {
                break;
            }
        }

        // Example 5 — KDebug timer (I)
        VDP_drawText( "Lesson 6: DEBUG                        ", 1, 1 );
        VDP_drawText( "Example 5: TIMERs                      ", 1, 3 );
        VDP_drawText( "                                       ", 1, 4 );
        VDP_drawText( " Press B to start the timer            ", 1, 5 );
        VDP_drawText( "                                       ", 1, 6 );
        VDP_drawText( "        -------                        ", 1, 7 );
        VDP_drawText( " ######|   |   |#######                ", 1, 8 );
        VDP_drawText( " ######|   |-- |#######                ", 1, 9 );
        VDP_drawText( " ######|   |   |#######                ", 1, 10 );
        VDP_drawText( "        -------                        ", 1, 11 );
        VDP_drawText( "                                       ", 1, 12 );
        VDP_drawText( " KDebug_StartTimer();                  ", 1, 13 );
        VDP_drawText( " precision: 7,610,000 cycles/sec       ", 1, 14 );

        VDP_drawText( "Press -B- to START the timer           ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_B ) {
                break;
            }
        }

        KLog( "--- TIMER STARTED ---" );
        KDebug_StartTimer();

        VDP_drawText( " Press C to stop the timer             ", 1, 5 );
        VDP_drawText( "Press -C- to STOP the timer            ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_C ) {
                break;
            }
        }

        KLog( "--- TIMER STOPPED ---" );
        KDebug_StopTimer();

        VDP_drawText( " Check the time in the Debug window    ", 1, 5 );
        VDP_drawText( " Divide result by 7,610,000 = seconds  ", 1, 6 );
        VDP_drawText( "        -------                        ", 1, 7 );
        VDP_drawText( " ######|   |   |#######                ", 1, 8 );
        VDP_drawText( " ######| - 0 - |#######                ", 1, 9 );
        VDP_drawText( " ######|   |   |#######                ", 1, 10 );
        VDP_drawText( "        -------                        ", 1, 11 );
        VDP_drawText( "                                       ", 1, 12 );
        VDP_drawText( " KDebug_StopTimer();                   ", 1, 13 );
        VDP_drawText( "                                       ", 1, 14 );

        VDP_drawText( "Press -A- to continue                  ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_A ) {
                break;
            }
        }

        // Example 6 — getSubTick() timer (II)
        // u32 getSubTick() — returns elapsed subticks since console reset
        VDP_drawText( "Lesson 6: DEBUG                        ", 1, 1 );
        VDP_drawText( "Example 6: TIMERs (II)                 ", 1, 3 );
        VDP_drawText( "                                       ", 1, 4 );
        VDP_drawText( " There are other timer types available.", 1, 5 );
        VDP_drawText( "                                       ", 1, 6 );
        VDP_drawText( "        -------                        ", 1, 7 );
        VDP_drawText( " ######|   |   |#######                ", 1, 8 );
        VDP_drawText( " ######|   |-- |#######                ", 1, 9 );
        VDP_drawText( " ######|   |   |#######                ", 1, 10 );
        VDP_drawText( "        -------                        ", 1, 11 );
        VDP_drawText( "                                       ", 1, 12 );
        VDP_drawText( " getSubTick();                         ", 1, 13 );
        VDP_drawText( " precision: 76,800 subticks/sec        ", 1, 14 );

        VDP_drawText( "Press -B- to START the timer           ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_B ) {
                break;
            }
        }

        KLog( "--- TIMER STARTED ---" );
        u32 startTime = getSubTick();

        VDP_drawText( " Press C to stop the timer             ", 1, 5 );
        VDP_drawText( "Press -C- to STOP the timer            ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_C ) {
                break;
            }
        }

        // unlike KDebug_StopTimer, we get the result as a value we can use
        KLog( "--- TIMER STOPPED ---" );
        u32 endTime = getSubTick();

        VDP_drawText( " Unlike the previous timer,            ", 1, 5 );
        VDP_drawText( " we can use the result programmatically", 1, 6 );
        VDP_drawText( "        -------                        ", 1, 7 );
        VDP_drawText( " ######|   |   |#######                ", 1, 8 );
        VDP_drawText( " ######| - 0 - |#######                ", 1, 9 );
        VDP_drawText( " ######|   |   |#######                ", 1, 10 );
        VDP_drawText( "        -------                        ", 1, 11 );
        VDP_drawText( "                                       ", 1, 12 );
        VDP_drawText( " getSubTick();                         ", 1, 13 );
        VDP_drawText( " Time:                     subticks    ", 1, 14 );
        VDP_drawText( "                           seconds     ", 1, 15 );

        char resultStr[16];
        uintToStr( endTime - startTime, resultStr, 16 );
        VDP_drawText( resultStr, 10, 14 );

        uintToStr( ( endTime - startTime ) / SUBTICKPERSECOND, resultStr, 16 );
        VDP_drawText( resultStr, 10, 15 );

        KLog_U1( "subticks: ", ( endTime - startTime ) );
        KLog_U1( "seconds:  ", ( endTime - startTime ) / SUBTICKPERSECOND );

        VDP_drawText( "Press -A- to continue                  ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_A ) {
                break;
            }
        }

        // Example 7 — tools.h timers (III)
        // up to 16 independent timers available (indices 0-15)
        VDP_drawText( "Lesson 6: DEBUG                        ", 1, 1 );
        VDP_drawText( "Example 7: TIMERs (III)                ", 1, 3 );
        VDP_drawText( "                                       ", 1, 4 );
        VDP_drawText( " Now using startTimer()                ", 1, 5 );
        VDP_drawText( " Up to 16 timers available (0-15)      ", 1, 6 );
        VDP_drawText( "        -------                        ", 1, 7 );
        VDP_drawText( " ######|   |   |#######                ", 1, 8 );
        VDP_drawText( " ######|   |-- |#######                ", 1, 9 );
        VDP_drawText( " ######|   |   |#######                ", 1, 10 );
        VDP_drawText( "        -------                        ", 1, 11 );
        VDP_drawText( "                                       ", 1, 12 );
        VDP_drawText( " Press -B- to start both timers        ", 1, 13 );
        VDP_drawText( " Timer 0 stops on -C-                  ", 1, 14 );
        VDP_drawText( " Timer 1 stops on -A-                  ", 1, 15 );

        VDP_drawText( "Press -B- to START both timers         ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_B ) {
                break;
            }
        }

        KLog( "--- TIMERS STARTED ---" );
        startTimer( 0 );
        startTimer( 1 );

        VDP_drawText( " Press C to stop timer 0               ", 1, 5 );
        VDP_drawText( "Press -C- to STOP timer 0              ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_C ) {
                break;
            }
        }

        // read timer 0 and stop it (TRUE = stop after reading)
        KLog( "--- TIMER 0 STOPPED ---" );
        u32 timer0 = getTimer( 0, TRUE );

        VDP_drawText( " Press A to stop timer 1               ", 1, 5 );
        VDP_drawText( "Press -A- to STOP timer 1              ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_A ) {
                break;
            }
        }

        // read timer 1 and stop it
        KLog( "--- TIMER 1 STOPPED ---" );
        u32 timer1 = getTimer( 1, TRUE );

        VDP_drawText( "        -------                        ", 1, 5 );
        VDP_drawText( " ######|   |   |#######                ", 1, 6 );
        VDP_drawText( " ######| - 0 - |#######                ", 1, 7 );
        VDP_drawText( " ######|   |   |#######                ", 1, 8 );
        VDP_drawText( "        -------                        ", 1, 9 );
        VDP_drawText( "                                       ", 1, 10 );
        VDP_drawText( " Timer 0                    subticks   ", 1, 11 );
        VDP_drawText( "                            seconds    ", 1, 12 );
        VDP_drawText( "                                       ", 1, 13 );
        VDP_drawText( " Timer 1                    subticks   ", 1, 14 );
        VDP_drawText( "                            seconds    ", 1, 15 );

        // display timer 0 results
        uintToStr( timer0, resultStr, 16 );
        VDP_drawText( resultStr, 10, 11 );

        uintToStr( timer0 / SUBTICKPERSECOND, resultStr, 16 );
        VDP_drawText( resultStr, 10, 12 );

        KLog_U1( "subticks: ", timer0 );
        KLog_U1( "seconds:  ", timer0 / SUBTICKPERSECOND );

        // display timer 1 results
        uintToStr( timer1, resultStr, 16 );
        VDP_drawText( resultStr, 10, 14 );

        uintToStr( timer1 / SUBTICKPERSECOND, resultStr, 16 );
        VDP_drawText( resultStr, 10, 15 );

        KLog_U1( "subticks: ", timer1 );
        KLog_U1( "seconds:  ", timer1 / SUBTICKPERSECOND );

        VDP_drawText( "Press -B- to continue                  ", 1, 20 );
        while ( TRUE ) {
            value = JOY_readJoypad( JOY_1 );
            if ( value & BUTTON_B ) {
                break;
            }
        }

        // soft reset
        reset();

        SYS_doVBlankProcess();

    }

    return 0;

}

void reset( void ) {
    asm( "move   #0x2700,%sr\n\t"
         "move.l (0),%a7\n\t"
         "move.l (4),%a0\n\t"
         "jmp    (%a0)" );
}
