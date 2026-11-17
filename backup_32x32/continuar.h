u8 modoFin = 0, opFin = 0;
u16 timerFin = 600;
void abrirContinuar() {
    modoFin = 1; opFin = 0; timerFin = 600; XGM_stopPlay();
    VDP_clearPlane(BG_A, TRUE); VDP_setTextPalette(PAL3);
    VDP_drawText("== C O N T I N U A R ? ==", 7, 9);
    VDP_drawText("> CONTINUAR", 13, 12);
    VDP_setTextPalette(PAL0); VDP_drawText("  FINALIZAR", 13, 14);
}
void abrirGameOver() {
    char b[42]; modoFin = 2;
    VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE); SPR_reset(); SPR_update();
    SPR_setVisibility(jugador.spr, HIDDEN); SPR_setVisibility(sprEscudo, HIDDEN);
    for(int i=0; i<MAX_ENEMIGOS; i++) if(enemigos[i].activo) SPR_setVisibility(enemigos[i].spr, HIDDEN);
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
    VDP_setTextPalette(PAL3); VDP_drawText("===== G A M E   O V E R =====", 5, 8);
    VDP_setTextPalette(PAL0); sprintf(b, "PUNTOS: %06lu   STAGE %d", puntos, faseActual + 1); VDP_drawText(b, 8, 12);
    VDP_setTextPalette(PAL3); VDP_drawText("PULSA START PARA SALIR", 9, 16);
}
u8 cicloFinJuego(u16 m, u16 prevM) {
    char t[24];
    if (!modoFin) return 0;
    if (modoFin == 1) {
        if ((m & (BUTTON_UP | BUTTON_DOWN)) && !(prevM & (BUTTON_UP | BUTTON_DOWN))) {
            opFin = !opFin; sfxStart();
            VDP_setTextPalette(opFin ? PAL0 : PAL3); VDP_drawText(opFin ? "  CONTINUAR" : "> CONTINUAR", 13, 12);
            VDP_setTextPalette(opFin ? PAL3 : PAL0); VDP_drawText(opFin ? "> FINALIZAR" : "  FINALIZAR", 13, 14);
        }
        if (timerFin > 0) timerFin--;
        VDP_setTextPalette(PAL3); sprintf(t, "TIEMPO: %2d", (timerFin + 59) / 60); VDP_drawText(t, 15, 17);
        if (!timerFin) { abrirGameOver(); return 1; }
        if ((m & (BUTTON_START | BUTTON_A | BUTTON_B | BUTTON_C)) && !(prevM & (BUTTON_START | BUTTON_A | BUTTON_B | BUTTON_C))) {
            sfxStart();
            if (!opFin) { modoFin = 0; vidas = 3; escudo = 2; puntos = 0; ebInvul = 90; VDP_clearPlane(BG_A, TRUE); SPR_setVisibility(sprEscudo, VISIBLE); verHUD(); XGM_setLoopNumber(-1); XGM_startPlay(jefeAct ? mus_jefe : mus_juego); }
            else abrirGameOver();
        }
        return 1;
    }
    if ((m & BUTTON_START) && !(prevM & BUTTON_START)) SYS_hardReset();
    return 1;
}
