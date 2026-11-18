#define MAX_EB 4
s16 ebX[MAX_EB], ebY[MAX_EB];
u8 ebAct[MAX_EB];
Sprite *sprEB[MAX_EB];
u8 ebInvul = 0;

void cicloBalasEnemigas() {
    if (ebInvul > 0) ebInvul--;
    for (int k = 0; k < MAX_EB; k++) {
        if (ebAct[k]) {
            ebY[k] += 3;
            if (ebY[k] > 228 || estadoJuego != 2) {
                ebAct[k] = 0;
                if (sprEB[k]) SPR_setVisibility(sprEB[k], HIDDEN);
            } else {
                if (!sprEB[k]) sprEB[k] = SPR_addSprite(&spr_bala_jefe, ebX[k], ebY[k], TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
                SPR_setPosition(sprEB[k], ebX[k], ebY[k]);
                SPR_setVisibility(sprEB[k], VISIBLE);
                if (!ebInvul && !jefeCine && vidas > 0 && abs((ebX[k] + 8) - (jugador.x + 12)) < 11 && abs((ebY[k] + 8) - (jugador.y + 12)) < 11) {
                    ebAct[k] = 0; SPR_setVisibility(sprEB[k], HIDDEN);
                    ebInvul = 45; sfxBoom(); crearExp(jugador.x, jugador.y);
                    if (escudo > 0) { escudo--; if (escudo == 0) SPR_setVisibility(sprEscudo, HIDDEN); }
                    else { nivelArma = 1; if (vidas > 0) vidas--; }
                    verHUD();
                    if (vidas == 0) { VDP_setTextPalette(PAL0); VDP_drawText("F I N   D E L   J U E G O", 8, 12); VDP_drawText("PULSA START", 15, 14); }
                }
            }
        }
    }
    if (estadoJuego == 2 && faseActual >= 2 && !jefeAct && !jefeCine && vidas > 0 && (timer % 48) == 0) {
        for (int i = 0; i < MAX_ENEMIGOS; i++) {
            if (enemigos[i].activo && enemigos[i].y > 16 && enemigos[i].y < 105) {
                for (int k = 0; k < MAX_EB; k++) {
                    if (!ebAct[k]) {
                        ebX[k] = enemigos[i].x + 4; ebY[k] = enemigos[i].y + 16; ebAct[k] = 1;
                        break;
                    }
                }
                break;
            }
        }
    }
}
