#ifndef FINAL_JUEGO_H
#define FINAL_JUEGO_H
#include <genesis.h>
#include "resources.h"
static u16 jefeCineTimer = 0;
static const char* txtCred(u8 i) {
    switch (i) {
        case 0: return "ETERNAL DREAM"; case 2: return "CREADOR"; case 3: return "FRANCOIS JEAN SOLES";
        case 5: return "CREADO CON AYUDA"; case 6: return "DE LA IA DE GOOGLE";
        case 8: return "DISENO Y CODIGO"; case 9: return "MEGA DRIVE SEGA 86";
        case 11: return "ARTE Y GRAFICOS"; case 12: return "EDICION 16 BITS";
        case 14: return "MUSICA Y SONIDO"; case 15: return "YM2612 + PSG";
        case 17: return "GRACIAS POR JUGAR"; case 18: return "FIN";
        default: return 0;
    }
}
static void mostrarPantallaFinal(u32 pts, u8 vid, u8 st) {
    (void)pts; (void)vid; (void)st;
    SPR_reset(); VDP_setVerticalScroll(BG_A, 0); VDP_setVerticalScroll(BG_B, 0);
    VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE);
    VDP_setTextPlane(BG_A); VDP_setTextPalette(PAL1);
    VDP_drawText("MISION CUMPLIDA", 12, 12); VDP_drawText("LA TIERRA ESTA A SALVO", 9, 14);
}
void actualizarCreditosFinal(u16 t) {
    if (t == 220) {
        u16 p[64]; extern s16 scrollY; scrollY = 0; XGM_setLoopNumber(-1); XGM_startPlay(mus_titulo); SPR_reset(); VDP_setVerticalScroll(BG_A, 0); VDP_setVerticalScroll(BG_B, 0);
        VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE); VDP_setTextPlane(BG_A); VDP_setTextPalette(PAL1);
        u16 ind = TILE_USER_INDEX;
        VDP_drawImageEx(BG_B, &img_ftop, TILE_ATTR_FULL(PAL0,0,0,0,ind), 0, 0, 0, 1); ind += img_ftop.tileset->numTile;
        VDP_drawImageEx(BG_B, &img_fmid, TILE_ATTR_FULL(PAL2,0,0,0,ind), 0, 9, 0, 1); ind += img_fmid.tileset->numTile;
        VDP_drawImageEx(BG_B, &img_fbot, TILE_ATTR_FULL(PAL3,0,0,0,ind), 0, 23, 0, 1);
        memcpy(p, img_ftop.palette->data, 32); memcpy(p+16, img_fmid.palette->data, 32);
        memcpy(p+32, img_fmid.palette->data, 32); memcpy(p+48, img_fbot.palette->data, 32);
        p[31] = 0x0EEE; PAL_setColors(0, p, 64, CPU);
    } else if (t > 220 && t <= 1020 && ((t - 220) & 15) == 0) {
        u16 st = (t - 220) / 16; VDP_clearTextArea(1, 0, 19, 28);
        for (u8 i = 0; i < 19; i++) { s16 y = 27 + (s16)(i * 2) - (s16)st; const char* s = txtCred(i); if (s && y >= 0 && y < 28) VDP_drawText(s, 1, (u16)y); }
    } else if (t == 1040) { VDP_clearTextArea(1, 0, 19, 28); VDP_drawText("FIN", 7, 13); VDP_drawText("PULSA START", 4, 16); }
    if (t >= 220) {
        scrollY = 0; VDP_setVerticalScroll(BG_B, 0);
        u16 idx = ((t - 220) >> 4) & 3, t1 = TILE_USER_INDEX + img_ftop.tileset->numTile;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2,0,0,(idx==2), t1 + img_fmid.tilemap->tilemap[(3+(idx&1))*40+31]), 31, 12);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2,0,0,(idx==1), t1 + img_fmid.tilemap->tilemap[4*40+31]), 31, 13);
    }
}
#endif
