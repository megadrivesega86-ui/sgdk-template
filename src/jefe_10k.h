Sprite *sprJefe = NULL, *sprJB[2] = {NULL, NULL};
s16 jefeX = 128, jefeY = -48, jefeVX = 2, jefeVida = 0, jbX[2], jbY[2];
u8 jefeAct = 0, jefeMuriendo = 0, jefeVencido = 0, jefeTiro = 0, jbAct[2] = {0, 0};

void verBarraJefe() {
    char b[28]; int n = (jefeVida > 0) ? (jefeVida + 2) / 3 : 0;
    strcpy(b, "JEFE [--------------------]");
    for (int i = 0; i < n && i < 20; i++) b[6 + i] = 61;
    VDP_setTextPalette(PAL0); VDP_drawText(b, 6, 2);
}
void resetJefe10k() {
    VDP_clearText(6, 2, 28);
    if (sprJefe) { SPR_releaseSprite(sprJefe); sprJefe = NULL; }
    for (u8 k = 0; k < 2; k++) { jbAct[k] = 0; if (sprJB[k]) SPR_setVisibility(sprJB[k], HIDDEN); }
    jefeAct = 0; jefeMuriendo = 0; jefeVencido = 0; jefeVida = 0;
}

void cicloJefe10k(s16 px, s16 py) {
    if (puntos >= 10000 && !jefeAct && !jefeMuriendo && !jefeVencido) {
        jefeAct = 1; jefeVida = 60; jefeX = 128; jefeY = -48; jefeVX = 2; jefeTiro = 0;
        PAL_setPalette(PAL3, spr_jefe1.palette->data, CPU);
        sprJefe = SPR_addSprite(&spr_jefe1, jefeX, jefeY, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
        for (int i = 0; i < MAX_ENEMIGOS; i++) { enemigos[i].activo = FALSE; SPR_setVisibility(enemigos[i].spr, HIDDEN); }
        sfxStart(); VDP_setTextPalette(PAL0); VDP_drawText("ALERTA: JEFE FINAL!", 10, 12); verBarraJefe();
    }
    if (jefeAct) {
        if (jefeY < 26) { jefeY++; if (jefeY == 26) VDP_clearText(10, 12, 20); }
        else { jefeX += jefeVX; if (jefeX < 16 || jefeX > 240) jefeVX = -jefeVX; }
        SPR_setPosition(sprJefe, jefeX, jefeY);
        if (++jefeTiro >= 40 && jefeY >= 26) {
            jefeTiro = 0;
            for (u8 k = 0; k < 2; k++) if (!jbAct[k]) {
                jbAct[k] = 1; jbX[k] = jefeX + (k ? 46 : 14); jbY[k] = jefeY + 42;
                if (!sprJB[k]) sprJB[k] = SPR_addSprite(&spr_laser, jbX[k], jbY[k], TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
                else { SPR_setVisibility(sprJB[k], VISIBLE); SPR_setPosition(sprJB[k], jbX[k], jbY[k]); }
            }
        }
        for (int i = 0; i < MAX_BALAS; i++) if (balas[i].activo) {
            if (balas[i].x < jefeX + 62 && balas[i].x + 6 > jefeX + 2 && balas[i].y < jefeY + 44 && balas[i].y + 10 > jefeY + 4) {
                balas[i].activo = FALSE; SPR_setVisibility(balas[i].spr, HIDDEN);
                crearExp(balas[i].x, balas[i].y); sfxBoom();
                jefeVida--; verBarraJefe(); if (jefeVida <= 0) { VDP_clearText(6, 2, 28);
                    jefeAct = 0; jefeMuriendo = 120;
                    for (u8 k = 0; k < 2; k++) { jbAct[k] = 0; if (sprJB[k]) SPR_setVisibility(sprJB[k], HIDDEN); }
                    break;
                }
            }
        }
    }
    for (u8 k = 0; k < 2; k++) if (jbAct[k]) {
        jbY[k] += 4; SPR_setPosition(sprJB[k], jbX[k], jbY[k]);
        if (jbY[k] > 230) { jbAct[k] = 0; SPR_setVisibility(sprJB[k], HIDDEN); }
        else if (px + 6 < jbX[k] + 6 && px + 26 > jbX[k] && py + 6 < jbY[k] + 10 && py + 26 > jbY[k]) {
            jbAct[k] = 0; SPR_setVisibility(sprJB[k], HIDDEN); sfxBoom();
            if (escudo > 0) { escudo--; if (escudo == 0) SPR_setVisibility(sprEscudo, HIDDEN); }
            else { nivelArma = 1; if (vidas > 0) vidas--; }
            verHUD();
        }
    }
    if (jefeMuriendo > 0) {
        jefeMuriendo--;
        SPR_setPosition(sprJefe, jefeX + ((jefeMuriendo & 2) ? 4 : -4), jefeY + ((jefeMuriendo & 4) ? 2 : -2));
        SPR_setVisibility(sprJefe, (jefeMuriendo & 1) ? VISIBLE : HIDDEN);
        if ((jefeMuriendo % 10) == 0) { sfxBoom(); crearExp(jefeX + (random() & 31) + 8, jefeY + (random() & 31)); }
        if (jefeMuriendo == 0) {
            SPR_releaseSprite(sprJefe); sprJefe = NULL; jefeVencido = 1;
            puntos += 5000; verHUD(); sfxStart();
            VDP_setTextPalette(PAL0); VDP_drawText("  JEFE DESTRUIDO!   ", 10, 12);
        }
    }
}
