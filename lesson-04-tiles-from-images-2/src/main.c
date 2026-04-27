/**
 *      @Title:  Leccion 04 - "Tiles a partir de imagenes (2)"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>
#include <resources.h>

int main( bool hard ) {
    
    //para llevar la cuenta de tiles en VRAM
    u16 ind;

    //recoge la paleta de la imagen y la asigna a la PAL0
    PAL_setPalette(PAL0, my64tiles.palette->data, CPU);

    //esta vez comenzamos a meter tiles en VRAM desde la segunda posici�n
    //dejamos el primer tile para pintar el fondo
    ind = 1;

    // carga la imagen en VRAM y la dibuja en pantalla en la posici�n (3,3)
    VDP_drawImageEx(BG_B, &my64tiles, TILE_ATTR_FULL(PAL0, 0, 0, 0, ind), 3, 3, 0, CPU);

    //incrementa ind para 'apuntar' a una zona de VRAM libre para futuras tiles
    ind += my64tiles.tileset->numTile;

    while ( TRUE ) {
        SYS_doVBlankProcess();
    }

    return 0;

}
