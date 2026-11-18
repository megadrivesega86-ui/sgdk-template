extern u8 ebInvul, jefeAct;
u8 modoFin = 0, opFin = 0; u16 timerFin = 0;
void abrirContinuar() {
    u8 y; static const u32 blk[] = {0xEEEEEEEE,0xEEEEEEEE,0xEEEEEEEE,0xEEEEEEEE,0xEEEEEEEE,0xEEEEEEEE,0xEEEEEEEE,0xEEEEEEEE};
    modoFin = 1; opFin = 0; timerFin = 599; XGM_stopPlay();
    for (y = 0; y < MAX_ENEMIGOS; y++) { enemigos[y].activo = FALSE; SPR_setVisibility(enemigos[y].spr, HIDDEN); }
    for (y = 0; y < MAX_EXP; y++) { exps[y].activo = FALSE; SPR_setVisibility(exps[y].spr, HIDDEN); }
    scrollY = 0; VDP_setVerticalScroll(BG_B, 0);
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x04081E)); PAL_setColor(14, RGB24_TO_VDPCOLOR(0x04081E)); PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF));
    VDP_loadTileData(blk, 1401, 1, CPU);
    VDP_clearTileMapRect(BG_B, 9, 8, 22, 11);
    VDP_fillTileMapRect(BG_A, TILE_ATTR_FULL(PAL0, 1, 0, 0, 1401), 9, 8, 22, 11);
    VDP_setTextPriority(TRUE); VDP_setTextPalette(PAL3);
    VDP_drawText("+====================+", 9, 8);
    for (y = 9; y < 18; y++) { VDP_drawText("|", 9, y); VDP_drawText("|", 30, y); }
    VDP_drawText("+====================+", 9, 18);
    VDP_drawText("==  CONTINUAR?  ==", 11, 10);
    VDP_drawText("> CONTINUAR", 13, 12);
    VDP_setTextPalette(PAL0); VDP_drawText("  FINALIZAR", 13, 14);
    VDP_setTextPalette(PAL3); VDP_drawText("TIEMPO: 10", 15, 16);
}
void abrirGameOver() {
    modoFin = 2; XGM_stopPlay();
    VDP_clearPlane(BG_A, TRUE); const Image* bgF = (faseActual == 2) ? &img_estrato : ((faseActual == 1) ? &img_altura : &img_tierra); VDP_drawImageEx(BG_B, bgF, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE_USER_INDEX), 0, 0, 0, 1); VDP_drawImageEx(BG_B, bgF, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE_USER_INDEX), 32, 0, 0, 1); VDP_clearPlane(BG_B, TRUE); SPR_reset(); SPR_update();
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000)); PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF));
    VDP_setTextPriority(FALSE); VDP_setTextPalette(PAL3); VDP_drawText("F I N   D E L   J U E G O", 8, 11);
    VDP_setTextPalette(PAL0); VDP_drawText("PULSA START", 14, 14);
}
u8 cicloFinJuego(u16 m, u16 prevM) {
    char t[4]; u8 seg; if (!modoFin) return 0;
    if (modoFin == 1) {
        if ((m & (BUTTON_UP | BUTTON_DOWN)) && !(prevM & (BUTTON_UP | BUTTON_DOWN))) {
            opFin = !opFin; sfxStart();
            VDP_setTextPalette(opFin ? PAL0 : PAL3); VDP_drawText(opFin ? "  CONTINUAR" : "> CONTINUAR", 13, 12);
            VDP_setTextPalette(opFin ? PAL3 : PAL0); VDP_drawText(opFin ? "> FINALIZAR" : "  FINALIZAR", 13, 14);
        }
        if (timerFin > 0) timerFin--;
        seg = (timerFin + 59) / 60; t[0] = (seg >= 10) ? '1' : ' '; t[1] = '0' + (seg % 10); t[2] = 0;
        VDP_setTextPalette(PAL3); VDP_drawText(t, 23, 16);
        if (!timerFin) { abrirGameOver(); return 1; }
        if ((m & (BUTTON_START | BUTTON_A | BUTTON_B | BUTTON_C)) && !(prevM & (BUTTON_START | BUTTON_A | BUTTON_B | BUTTON_C))) {
            sfxStart();
            if (!opFin) { modoFin = 0; vidas = 3; escudo = 2; ebInvul = 90; VDP_setTextPriority(FALSE); PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000)); VDP_clearPlane(BG_A, TRUE); const Image* bgF = (faseActual == 2) ? &img_estrato : ((faseActual == 1) ? &img_altura : &img_tierra); VDP_drawImageEx(BG_B, bgF, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE_USER_INDEX), 0, 0, 0, 1); VDP_drawImageEx(BG_B, bgF, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE_USER_INDEX), 32, 0, 0, 1); SPR_setVisibility(sprEscudo, VISIBLE); verHUD(); XGM_setLoopNumber(-1); XGM_startPlay(jefeAct ? mus_jefe : mus_juego); }
            else abrirGameOver();
        }
        return 1;
    }
    if ((m & BUTTON_START) && !(prevM & BUTTON_START)) SYS_hardReset();
    return 1;
}
