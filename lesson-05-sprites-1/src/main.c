/**
 *      @Title:  Leccion 05 - "sprites (1)"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>
#include "sprites.h"  //para generar el .h a partir del .res

//Declaracion de funciones
static void handleInput( void );
static void pinta_posicion( void );

//Puntero a los sprites y puntero a puntero de sprites
Sprite* mi_sprite_cuadrado;
Sprite* mi_sprite_sonic;
Sprite* mi_sprite;

//Posicion en pantalla del sprite
int posx;
int posy;

int main( bool hard ) {

    //Posicion inicial del sprite
    posx = 0;
    posy = 0;

    //pone la pantalla a 320x224
    VDP_setScreenWidth320();

    //Inicializa motor de sprites con los par�metros por defecto
    SPR_init();

    //Recoje la paleta del sprite y la mete en la 2a paleta del sistema
    //ojo que si lo hacemos con la PAL0 el texto de las coordenadas sale EN NEGRO y no se ver�
    PAL_setPalette(PAL1,sprite_8x8.palette->data, CPU);

    //Crea sprites
    mi_sprite_cuadrado = SPR_addSprite(&sprite_8x8, posx, posy, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    mi_sprite_sonic    = SPR_addSprite(&sprite_sonic, posx, posy, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

        //El puntero (a puntero) apunta al sprite actual: en el inicio, el cuadrado
        mi_sprite = mi_sprite_cuadrado;

    //Bucle principal
    while(TRUE)
    {
        //recoje la entrada de los mandos
        handleInput();

        //escribe la posicion del sprite
        pinta_posicion();

        //actualiza el VDP
        SPR_update();

        //sincroniza la Megadrive con la TV
        SYS_doVBlankProcess();
    }

    return 0;
}

//Funcion handleInput() : recoje la entrada del mando (pad) y actualiza la posicion
//del sprite, sumando o restando de la variable posx y de poxsy
//Adem�s cambia de sprite 'al vuelo'
static void handleInput( void )
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
        SPR_setPosition(mi_sprite, posx--, posy);
    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
        SPR_setPosition(mi_sprite, posx++, posy);
    //si pulsamos arriba...
    if (value & BUTTON_UP)
        SPR_setPosition(mi_sprite, posx, posy--);
    //si pulsamos abajo...
    if (value & BUTTON_DOWN)
        SPR_setPosition(mi_sprite, posx, posy++);

    //si pulsamos A...
    if ((value & BUTTON_A)&&(mi_sprite == mi_sprite_sonic)){
        mi_sprite = mi_sprite_cuadrado;
        SPR_setPosition(mi_sprite, posx, posy); //necesario para q actualice al momento la TV
        }
    //si pulsamos B...
    if ((value & BUTTON_B)&&(mi_sprite == mi_sprite_cuadrado)){
        mi_sprite = mi_sprite_sonic;
        SPR_setPosition(mi_sprite, posx, posy);
        }
}

//Funci�m pinta_posicion() : Escribe en pantalla la posici�n del sprite
static void pinta_posicion( void )
{
    //declaramos una cadena de caracteres
    //sprintf : pasa un valor numerico(posx / posy) a caracteres y los copia
    //en la cadena anterior. %4d alinea a la derecha, importante para al pasar
    //de numeros negativos a positivos todo salga correctamente

    char cadena1[32];
    sprintf(cadena1, "x:  %4d", posx);
    VDP_drawText(cadena1, 2, 23);

    char cadena2[32];
    sprintf(cadena2, "y:  %4d", posy);
    VDP_drawText(cadena2, 2, 24);

}
