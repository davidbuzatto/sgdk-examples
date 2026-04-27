/**
 *      @Title:  Leccion 07 - "deconstruyendo escenarios"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>
#include "gfx.h"     //carga la imagen de background

int main( bool hard ) {
    u16 ind, contador;

    SYS_disableInts();

    VDP_setScreenWidth320();

    PAL_setPalette(PAL0,bga_image.palette->data, CPU);

    ind = TILE_USER_INDEX;
    VDP_drawImageEx(BG_A, &bga_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bga_image.tileset->numTile;

    SYS_enableInts();

    contador = 0;

    while(TRUE)
    {
        contador++; //a 60fps, cada segundo vale 60 m�s

        //Cambio de cielo (azul->naranja->rojo->negro)
        if(contador>60*1)     PAL_setColor(4, RGB24_TO_VDPCOLOR(0xFF6600));
        if(contador>60*2)     PAL_setColor(4, RGB24_TO_VDPCOLOR(0xCC5200));
        if(contador>60*3)     PAL_setColor(4, RGB24_TO_VDPCOLOR(0x993D00));
        if(contador>60*4)     PAL_setColor(4, RGB24_TO_VDPCOLOR(0x662900));
        if(contador>60*5)     PAL_setColor(4, RGB24_TO_VDPCOLOR(0x000000));

        //cambio de castillo (rojo->rojo oscuro)
        if(contador>60*3)     PAL_setColor(2,  RGB24_TO_VDPCOLOR(0x802000));
        if(contador>60*4)     PAL_setColor(12, RGB24_TO_VDPCOLOR(0x661A00));

        //cambio del cesped (verde->marr�n)
        if(contador>60*1)     PAL_setColor(8, RGB24_TO_VDPCOLOR(0x999900));
        if(contador>60*2)     PAL_setColor(1, RGB24_TO_VDPCOLOR(0x997300));

        SYS_doVBlankProcess();
    }

    return 0;
}
