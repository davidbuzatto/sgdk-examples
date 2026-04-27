/**
 *      @Title:  Leccion 01 - "Hello world"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>

int main( bool hard ) {

    //VDP_drawText("texto", x, y);  donde (x,y) se refiere a TILES y no a pixels (1 tile=8 pixels)
    VDP_drawText("Hello World!", 10, 13);

    while ( TRUE ) {

        //leer controles
        //mover sprites
        //actualizar marcadores
        //etc

        SYS_doVBlankProcess(); //The method actually wait for the next start of Vertical blanking.

    }

    //main() es de tipo INT y por tanto debe tener un return(0) para ser sint�cticamente correcta
    //sin embargo NUNCA deberemos llegar aqu�, puesto que un juego de consola no termina nunca
    //en el caso de llegar aqu�, se producir� un error de "INSTRUCCION ILEGAL"
    return 0;

}
