u8 jefeTipo = 0;
s16 jbVX[3];
Sprite *sprJefe = NULL, *sprJB[3] = {NULL, NULL};
s16 jefeX = 128, jefeY = -48, jefeVX = 2, jefeVida = 0, jbX[3], jbY[3];
u8 msgFase2Timer = 0, jefeIntro = 0, jefeCine = 0;
s16 cineVel = 0;
u8 jefeAct = 0, jefeMuriendo = 0, jefeVencido = 0, jefeTiro = 0, jbAct[3] = {0, 0};

void verBarraJefe() {
    char b[28]; int n = (jefeVida > 0) ? (jefeTipo == 2) ? ((jefeVida + 22) / 23) : ((jefeVida + 14) / 15) : 0;
    strcpy(b, "BOSS [....................]");
    for (int i = 0; i < n && i < 20; i++) b[6 + i] = 35;
    PAL_setColor(63, (jefeTipo == 2) ? RGB24_TO_VDPCOLOR(0xFF3300) : RGB24_TO_VDPCOLOR(0xFFCC00)); VDP_setTextPalette(PAL3); VDP_drawText(b, 6, 4); VDP_setTextPalette(PAL0);
}
void resetJefe10k() {
    VDP_clearText(6, 4, 28); msgFase2Timer = 0; VDP_clearText(10, 12, 20);
    if (sprJefe) { SPR_releaseSprite(sprJefe); sprJefe = NULL; }
    for (u8 k = 0; k < 3; k++) { jbAct[k] = 0; if (sprJB[k]) SPR_setVisibility(sprJB[k], HIDDEN); }
    jefeAct = 0; jefeMuriendo = 0; jefeIntro = 0; jefeCine = 0; cineVel = 0; jefeVencido = 0; jefeTipo = 0; jefeVida = 0;
}

void cicloJefe10k(s16 px, s16 py) {
    static u16 palNave[16];
    if (msgFase2Timer > 0 && --msgFase2Timer == 0) VDP_clearText(10, 12, 20);
    if (!jefeAct && !jefeMuriendo && !jefeIntro && !jefeCine && ((jefeVencido == 0 && puntos >= 10000) || (jefeVencido == 1 && faseActual == 1 && puntos >= 25000))) {
        jefeIntro = 60; XGM_stopPlay();
        for (int i = 0; i < MAX_ENEMIGOS; i++) { enemigos[i].activo = FALSE; SPR_setVisibility(enemigos[i].spr, HIDDEN); }
        VDP_setTextPalette(PAL0); jefeTipo = (jefeVencido == 0) ? 1 : 2; VDP_drawText((jefeTipo == 2) ? "ALERTA: ACORAZADO!" : "ALERTA: JEFE FINAL!", 10, 12);
    }
    if (jefeIntro > 0 && --jefeIntro == 0) {
        jefeAct = 1; jefeVida = (jefeTipo == 2) ? 450 : 300; jefeX = 128; jefeY = -48; jefeVX = (jefeTipo == 2) ? 3 : 2; jefeTiro = 0;
        sprJefe = SPR_addSprite((jefeTipo == 2) ? &spr_jefe2 : &spr_jefe1, jefeX, jefeY, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
        XGM_setLoopNumber(-1); XGM_startPlay(mus_jefe); verBarraJefe();
    }
    if (jefeAct) {
        if (jefeY < 26) { jefeY++; if (jefeY == 26) VDP_clearText(10, 12, 20); }
        else { jefeX += jefeVX; if (jefeX < 16 || jefeX > 240) jefeVX = -jefeVX; }
        SPR_setPosition(sprJefe, jefeX, jefeY);
        if (++jefeTiro >= 40 && jefeY >= 26) {
            jefeTiro = 0;
            for (u8 k = 0; k < 3; k++) if (!jbAct[k]) {
                jbAct[k] = 1; jbX[k] = jefeX + (k ? 46 : 14); jbY[k] = jefeY + 42;
                if (!sprJB[k]) sprJB[k] = SPR_addSprite(&spr_bala_jefe, jbX[k] - 4, jbY[k], TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
                else { SPR_setVisibility(sprJB[k], VISIBLE); SPR_setPosition(sprJB[k], jbX[k], jbY[k]); }
            }
        }
        for (int i = 0; i < MAX_BALAS; i++) if (balas[i].activo) {
            if (balas[i].x < jefeX + 62 && balas[i].x + 6 > jefeX + 2 && balas[i].y < jefeY + 44 && balas[i].y + 10 > jefeY + 4) {
                balas[i].activo = FALSE; SPR_setVisibility(balas[i].spr, HIDDEN);
                crearExp(balas[i].x, balas[i].y); sfxBoom();
                jefeVida--; verBarraJefe(); if (jefeVida <= 0) { VDP_clearText(6, 4, 28);
                    jefeAct = 0; jefeMuriendo = 180; XGM_stopPlay();
                    for (u8 k = 0; k < 3; k++) { jbAct[k] = 0; if (sprJB[k]) SPR_setVisibility(sprJB[k], HIDDEN); }
                    break;
                }
            }
        }
    }
    for (u8 k = 0; k < 3; k++) if (jbAct[k]) {
        jbY[k] += (jefeTipo == 2) ? 5 : 4; jbX[k] += jbVX[k]; SPR_setPosition(sprJB[k], jbX[k], jbY[k]);
        if (jbY[k] > 230 || jbX[k] < -16 || jbX[k] > 336) { jbAct[k] = 0; SPR_setVisibility(sprJB[k], HIDDEN); }
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
        if ((jefeMuriendo % 10) == 0) { if (jefeMuriendo >= 30 && (jefeMuriendo % 30) == 0) sfxBoom(); crearExp(jefeX + (random() & 31) + 8, jefeY + (random() & 31)); }
        if (jefeMuriendo == 0) {
            SPR_releaseSprite(sprJefe); sprJefe = NULL; jefeVencido = (jefeTipo == 2) ? 2 : 1;
            puntos += 5000; verHUD();
            XGM_setLoopNumber(-1); XGM_startPlay(mus_victoria);
            jefeCine = 1; cineVel = 0;
        }
    }
    if (jefeCine > 0) {
        SPR_setAnim(jugador.spr, 0); SPR_setHFlip(jugador.spr, FALSE);
        if (jefeCine == 1) {
            if (jugador.x < 143) jugador.x += 2; else if (jugador.x > 145) jugador.x -= 2; else jugador.x = 144;
            if (jugador.y < 175) jugador.y += 2; else if (jugador.y > 177) jugador.y -= 2; else jugador.y = 176;
            if (jugador.x == 144 && jugador.y == 176) {
                jefeCine = 2; cineVel = 2; sfxBoom();
                crearExp(jugador.x + 8, jugador.y + 24);
            }
        } else if (jefeCine == 2) {
            jugador.y -= cineVel; if (cineVel < 12) cineVel++;
            if ((cineVel & 1) == 0) crearExp(jugador.x + 8, jugador.y + 26);
            if (jugador.y < -32) jefeCine = 3;
        } else if (jefeCine == 3) {
            PAL_getPalette(PAL1, palNave); PAL_getPalette(PAL1, palNave); PAL_getPalette(PAL1, palNave); PAL_fadeOutAll(25, FALSE);
            u16 p[64]; memcpy(p, img_altura.palette->data, 32); memcpy(p+16, palNave, 32); memcpy(p+32, spr_enemigo.palette->data, 32); memcpy(p+48, spr_pwr.palette->data, 32); *(p+15) = RGB24_TO_VDPCOLOR(0xFFFFFF);
            VDP_drawImageEx(BG_B, (jefeVencido == 2) ? &img_estrato : &img_altura, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX), 0, 0, FALSE, TRUE);
            VDP_drawImageEx(BG_B, (jefeVencido == 2) ? &img_estrato : &img_altura, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX), 32, 0, FALSE, TRUE);
            faseActual = (jefeVencido == 2) ? 2 : 1; jugador.x = 144; jugador.y = -36;
            SPR_setPosition(jugador.spr, jugador.x, jugador.y);
            SPR_update();
            PAL_fadeInAll(p, 25, FALSE);
            jefeCine = 4;
        } else if (jefeCine == 4) {
            s16 dist = 176 - jugador.y;
            if (dist > 0) {
                s16 paso = (dist > 48) ? 6 : ((dist > 24) ? 4 : ((dist > 10) ? 2 : 1));
                jugador.y += paso;
            } else {
                jugador.y = 176; jefeCine = 0;
                VDP_setTextPalette(PAL0); VDP_drawText((faseActual == 2) ? "FASE 3: ESTRATOSFERA" : "FASE 2: ALTA ALTITUD", 10, 12);
                msgFase2Timer = 120; VDP_clearText(14, 14, 12);
                XGM_setLoopNumber(-1); XGM_startPlay(mus_juego);
            }
        }
        SPR_setPosition(jugador.spr, jugador.x, jugador.y);
        if (escudo > 0) SPR_setPosition(sprEscudo, jugador.x - 4, jugador.y - 4);
        for (int i = 0; i < MAX_EXP; i++) if (exps[i].activo && --exps[i].vida == 0) { exps[i].activo = FALSE; SPR_setVisibility(exps[i].spr, HIDDEN); }
    }
u16 p[64]; memcpy(p, img_altura.palette->data, 32); memcpy(p+16, palNave, 32); memcpy(p+32, spr_enemigo.palette->data, 32); memcpy(p+48, spr_pwr.palette->data, 32); *(p+15) = RGB24_TO_VDPCOLOR(0xFFFFFF);
            }