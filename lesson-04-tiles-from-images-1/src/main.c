/**
 *      @Title:  Leccion 04 - "Tiles a partir de imagenes"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>
#include <resources.h>

int main( bool hard ) {

    //para llevar la cuenta de tiles en VRAM
    u16 ind;

    //recoge la paleta de la imagen y la asigna a la PAL0
    PAL_setPalette(PAL0, moon.palette->data, CPU);

    //reserva las primeras 16 posiciones de la VRAM, de la 0 a la 15 (ind = 16)
    ind = TILE_USER_INDEX;

    // carga la imagen en VRAM y la dibuja en pantalla en la posici�n (3,3)
    VDP_drawImageEx(BG_A, &moon, TILE_ATTR_FULL(PAL0, 0, 0, 0, ind), 3, 3, 0, CPU);

    //incrementa ind para 'apuntar' a una zona de VRAM libre para futuras tiles
    ind += moon.tileset->numTile;

    while ( TRUE ) {
        SYS_doVBlankProcess();
    }

    return 0;
    
}
