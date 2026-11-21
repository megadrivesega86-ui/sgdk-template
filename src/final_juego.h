#ifndef FINAL_JUEGO_H
#define FINAL_JUEGO_H
u16 jefeCineTimer = 0;
void mostrarPantallaFinal(u32 pts, u8 vds, u8 nSel) {
    (void)nSel;
    SPR_reset(); SPR_update();
    VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE);
    VDP_setHorizontalScroll(BG_B, 0); VDP_setVerticalScroll(BG_B, 0);
    VDP_setHorizontalScroll(BG_A, 0); VDP_setVerticalScroll(BG_A, 0);
    PAL_setColor(0, 0x0000); PAL_setColor(15, 0x0FFF); PAL_setColor(31, 0x0FFF);
    char b[32]; VDP_setTextPalette(PAL0);
    VDP_drawText("MISSION COMPLETE!", 11, 8);
    VDP_drawText("PILOT: ETERNAL ACE", 11, 11);
    sprintf(b, "LIFE BONUS: %05lu", (u32)vds * 1000); VDP_drawText(b, 11, 14);
    sprintf(b, "FINAL SCORE: %06lu", pts + (u32)vds * 1000); VDP_drawText(b, 10, 17);
}
void actualizarCreditosFinal(u16 t) {
    if (t >= 220) {
        VDP_setVerticalScroll(BG_B, 0); VDP_setHorizontalScroll(BG_B, 0);
        VDP_setVerticalScroll(BG_A, 0); VDP_setHorizontalScroll(BG_A, 0);
        PAL_setPalette(PAL0, img_ptop.palette->data, DMA);
        PAL_setPalette(PAL2, img_pmid.palette->data, DMA);
        PAL_setPalette(PAL3, img_pbot.palette->data, DMA);
        PAL_setColor(0, 0x0000); PAL_setColor(31, 0x0FFF);
    }
    if (t == 220) {
        VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE);
        u16 i = TILE_USER_INDEX;
        VDP_drawImageEx(BG_B, &img_ptop, TILE_ATTR_FULL(PAL0,0,0,0,i), 0, 0, 0, 1); i += img_ptop.tileset->numTile;
        VDP_drawImageEx(BG_B, &img_pmid, TILE_ATTR_FULL(PAL2,0,0,0,i), 0, 9, 0, 1); i += img_pmid.tileset->numTile;
        VDP_drawImageEx(BG_B, &img_pbot, TILE_ATTR_FULL(PAL3,0,0,0,i), 0, 23, 0, 1);
        VDP_setTextPalette(PAL1);
    } else if (t == 260) { VDP_drawText("ETERNAL DREAM", 1, 2); VDP_drawText("PEACE RESTORED", 1, 4); }
    else if (t == 420) { VDP_clearTextArea(1,2,18,4); VDP_drawText("DESIGN & CODE", 1, 2); VDP_drawText("MEGADRIVE SEGA 86", 1, 4); }
    else if (t == 580) { VDP_clearTextArea(1,2,18,4); VDP_drawText("PIXEL ART & GFX", 1, 2); VDP_drawText("16-BIT EDITION", 1, 4); }
    else if (t == 740) { VDP_clearTextArea(1,2,18,4); VDP_drawText("MUSIC & SOUND", 1, 2); VDP_drawText("YM2612 + PSG", 1, 4); }
    else if (t == 900) { VDP_clearTextArea(1,2,18,4); VDP_drawText("THANK YOU!", 1, 2); VDP_drawText("THE END", 1, 4); }
    else if (t > 960) {
        if ((t & 31) == 0) VDP_drawText("PRESS START", 1, 6);
        else if ((t & 31) == 16) VDP_clearText(1, 6, 11);
    }
}
#endif
