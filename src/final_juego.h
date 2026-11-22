#ifndef FINAL_JUEGO_H
#define FINAL_JUEGO_H
u16 jefeCineTimer = 0;
void mostrarPantallaFinal(u32 pts, u8 vds, u8 nSel) {
    (void)nSel;
    SPR_reset(); SPR_update(); XGM_setLoopNumber(0); XGM_startPlay(mus_creditos);
    VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE);
    VDP_setHorizontalScroll(BG_B, 0); VDP_setVerticalScroll(BG_B, 0);
    VDP_setHorizontalScroll(BG_A, 0); VDP_setVerticalScroll(BG_A, 0);
    VDP_setBackgroundColor(63); PAL_setColor(63, 0x0000); PAL_setColor(0, 0x0000); PAL_setColor(15, 0x0FFF); PAL_setColor(31, 0x0FFF);
    char b[32]; VDP_setTextPalette(PAL0);
    VDP_drawText("MISION CUMPLIDA!", 12, 8);
    VDP_drawText("PILOTO: AS ETERNAL", 11, 11);
    sprintf(b, "BONUS VIDAS: %05lu", (u32)vds * 1000); VDP_drawText(b, 11, 14);
    sprintf(b, "PUNTOS TOTAL: %06lu", pts + (u32)vds * 1000); VDP_drawText(b, 10, 17);
}
const char* txtCred(u8 i) {
    switch(i) {
        case 0: return "ETERNAL DREAM"; case 2: return "DISENO Y CODIGO"; case 3: return "MEGADRIVE SEGA 86";
        case 5: return "ARTE Y GRAFICOS"; case 6: return "EDICION 16 BITS"; case 8: return "MUSICA Y SONIDO";
        case 9: return "YM2612 + PSG"; case 11: return "GRACIAS POR JUGAR"; case 12: return "FIN";
        default: return 0;
    }
}
void actualizarCreditosFinal(u16 t) {
    if (t >= 220) {
        VDP_setVerticalScroll(BG_B, 0); VDP_setHorizontalScroll(BG_B, 0);
        VDP_setVerticalScroll(BG_A, 0); VDP_setHorizontalScroll(BG_A, 0);
        PAL_setPalette(PAL0, img_ftop.palette->data, DMA);
        PAL_setPalette(PAL2, img_fmid.palette->data, DMA);
        PAL_setPalette(PAL3, img_fbot.palette->data, DMA);
        VDP_setBackgroundColor(63); PAL_setColor(63, 0x0000); PAL_setColor(0, 0x0000); PAL_setColor(31, 0x0FFF);
    }
    if (t == 220) {
        VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE);
        u16 i = TILE_USER_INDEX;
        VDP_drawImageEx(BG_B, &img_ftop, TILE_ATTR_FULL(PAL0,0,0,0,i), 0, 0, 0, 1); i += img_ftop.tileset->numTile;
        VDP_drawImageEx(BG_B, &img_fmid, TILE_ATTR_FULL(PAL2,0,0,0,i), 0, 9, 0, 1); i += img_fmid.tileset->numTile;
        VDP_drawImageEx(BG_B, &img_fbot, TILE_ATTR_FULL(PAL3,0,0,0,i), 0, 23, 0, 1);
        VDP_setTextPalette(PAL1);
    } else if (t == 260) { VDP_drawText("ETERNAL DREAM", 1, 2); VDP_drawText("GALAXIA A SALVO", 1, 4); }
        else if (t > 220 && t <= 820) {
            if (((t - 220) & 15) == 0) {
                u16 st = (t - 220) / 16;
                VDP_clearTextArea(1, 0, 19, 28);
                for (u8 i = 0; i < 13; i++) {
                    s16 y = 27 + (s16)(i * 2) - (s16)st;
                    const char* s = txtCred(i);
                    if (y >= 2 && y <= 26 && s) VDP_drawText(s, 1, y);
                }
            }
        } else if (t > 820) {
            if ((t & 31) == 0) VDP_drawText("PULSA START", 1, 17);
            else if ((t & 31) == 16) VDP_clearText(1, 17, 11);
        }
    }
#endif
