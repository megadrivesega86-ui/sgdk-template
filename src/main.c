#include <genesis.h>
#include "resources.h"

#define MAX_BALAS 12
#define MAX_ENEMIGOS 6
#define MAX_EXP 4
#define ID_SFX_LASER 64
#define ID_SFX_BOOM  65

typedef struct { Sprite* spr; s16 x, y; bool activo; u16 vida; } Entidad;
Entidad jugador, balas[MAX_BALAS], enemigos[MAX_ENEMIGOS], exps[MAX_EXP];
u32 puntos = 0;
u16 vidas = 3, cd = 0, timer = 0, prevM = 0;
bool autoFire = FALSE, gameOver = FALSE;
s16 scrollY = 0;
char hud;

void sfxLaser() { XGM_startPlayPCM(ID_SFX_LASER, 1, SOUND_PCM_CH2); }
void sfxBoom()  { XGM_startPlayPCM(ID_SFX_BOOM, 10, SOUND_PCM_CH3); }

void verHUD() {
    sprintf(hud, "PUNTOS:%05lu  VIDAS:%d  %s", puntos, vidas, autoFire ? "[AUTO]" : "      ");
    VDP_drawText(hud, 1, 1);
}

void crearExp(s16 x, s16 y) {
    for (int i = 0; i < MAX_EXP; i++) {
        if (!exps[i].activo) {
            exps[i].activo = TRUE; exps[i].x = x; exps[i].y = y; exps[i].vida = 12;
            SPR_setVisibility(exps[i].spr, VISIBLE); SPR_setPosition(exps[i].spr, x, y);
            break;
        }
    }
}

void disparar() {
    for (int i = 0; i < MAX_BALAS; i++) {
        if (!balas[i].activo) {
            balas[i].activo = TRUE; balas[i].x = jugador.x + 12; balas[i].y = jugador.y - 6;
            SPR_setVisibility(balas[i].spr, VISIBLE); SPR_setPosition(balas[i].spr, balas[i].x, balas[i].y);
            sfxLaser();
            break;
        }
    }
}

void crearEnemigo() {
    for (int i = 0; i < MAX_ENEMIGOS; i++) {
        if (!enemigos[i].activo) {
            enemigos[i].activo = TRUE; enemigos[i].x = (random() % 272) + 16; enemigos[i].y = -32;
            SPR_setVisibility(enemigos[i].spr, VISIBLE); SPR_setPosition(enemigos[i].spr, enemigos[i].x, enemigos[i].y);
            break;
        }
    }
}

bool choca(s16 x1, s16 y1, s16 w1, s16 h1, s16 x2, s16 y2, s16 w2, s16 h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}
void reiniciar() {
    puntos = 0; vidas = 3; gameOver = FALSE;
    jugador.x = 144; jugador.y = 170;
    SPR_setVisibility(jugador.spr, VISIBLE);
    for (int i = 0; i < MAX_BALAS; i++) { balas[i].activo = FALSE; SPR_setVisibility(balas[i].spr, HIDDEN); }
    for (int i = 0; i < MAX_ENEMIGOS; i++) { enemigos[i].activo = FALSE; SPR_setVisibility(enemigos[i].spr, HIDDEN); }
    VDP_clearTextArea(10, 12, 20, 4);
    verHUD();
}

int main() {
    JOY_init(); SPR_init();
    XGM_setPCM(ID_SFX_LASER, sfx_laser, sizeof(sfx_laser));
    XGM_setPCM(ID_SFX_BOOM, sfx_boom, sizeof(sfx_boom));

    PAL_setPalette(PAL1, spr_nave.palette->data, DMA);
    PAL_setPalette(PAL2, spr_enemigo.palette->data, DMA);
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    for (int i = 0; i < 45; i++) { VDP_drawText(".", random() % 40, random() % 28); }

    jugador.x = 144; jugador.y = 170;
    jugador.spr = SPR_addSprite(&spr_nave, jugador.x, jugador.y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
    for (int i = 0; i < MAX_BALAS; i++) { balas[i].spr = SPR_addSprite(&spr_laser, 0, 0, TILE_ATTR(PAL1, 0, FALSE, FALSE)); SPR_setVisibility(balas[i].spr, HIDDEN); }
    for (int i = 0; i < MAX_ENEMIGOS; i++) { enemigos[i].spr = SPR_addSprite(&spr_enemigo, 0, 0, TILE_ATTR(PAL2, 0, FALSE, FALSE)); SPR_setVisibility(enemigos[i].spr, HIDDEN); }
    for (int i = 0; i < MAX_EXP; i++) { exps[i].spr = SPR_addSprite(&spr_exp, 0, 0, TILE_ATTR(PAL1, 0, FALSE, FALSE)); SPR_setVisibility(exps[i].spr, HIDDEN); }
    verHUD();
    while(1) {
        u16 m = JOY_readJoypad(JOY_1);
        if (gameOver) {
            if ((m & BUTTON_START) && !(prevM & BUTTON_START)) { reiniciar(); }
            prevM = m; SPR_update(); SYS_doVBlankProcess(); continue;
        }
        scrollY -= 2; VDP_setVerticalScroll(BG_B, scrollY);
        if (m & BUTTON_LEFT)  { jugador.x -= 3; }
        if (m & BUTTON_RIGHT) { jugador.x += 3; }
        if (m & BUTTON_UP)    { jugador.y -= 3; }
        if (m & BUTTON_DOWN)  { jugador.y += 3; }
        if (jugador.x < 8) { jugador.x = 8; } if (jugador.x > 280) { jugador.x = 280; }
        if (jugador.y < 24) { jugador.y = 24; } if (jugador.y > 184) { jugador.y = 184; }
        SPR_setPosition(jugador.spr, jugador.x, jugador.y);

        if ((m & BUTTON_B) && !(prevM & BUTTON_B)) { autoFire = !autoFire; verHUD(); }
        prevM = m;
        if (cd > 0) { cd--; }
        if (((m & (BUTTON_A | BUTTON_C)) || autoFire) && cd == 0) { disparar(); cd = 7; }

        for (int i = 0; i < MAX_BALAS; i++) {
            if (balas[i].activo) {
                balas[i].y -= 6;
                if (balas[i].y < -8) { balas[i].activo = FALSE; SPR_setVisibility(balas[i].spr, HIDDEN); }
                else { SPR_setPosition(balas[i].spr, balas[i].x, balas[i].y); }
            }
        }
        for (int i = 0; i < MAX_EXP; i++) {
            if (exps[i].activo) {
                exps[i].vida--;
                if (exps[i].vida == 0) { exps[i].activo = FALSE; SPR_setVisibility(exps[i].spr, HIDDEN); }
            }
        }
        if (++timer >= 42) { crearEnemigo(); timer = 0; }
        for (int i = 0; i < MAX_ENEMIGOS; i++) {
            if (enemigos[i].activo) {
                enemigos[i].y += 2;
                if (enemigos[i].y > 224) { enemigos[i].activo = FALSE; SPR_setVisibility(enemigos[i].spr, HIDDEN); }
                else {
                    SPR_setPosition(enemigos[i].spr, enemigos[i].x, enemigos[i].y);
                    for (int b = 0; b < MAX_BALAS; b++) {
                        if (balas[b].activo && choca(balas[b].x, balas[b].y, 8, 8, enemigos[i].x+2, enemigos[i].y+2, 28, 28)) {
                            balas[b].activo = FALSE; SPR_setVisibility(balas[b].spr, HIDDEN);
                            enemigos[i].activo = FALSE; SPR_setVisibility(enemigos[i].spr, HIDDEN);
                            crearExp(enemigos[i].x, enemigos[i].y); sfxBoom();
                            puntos += 100; verHUD(); break;
                        }
                    }
                    if (enemigos[i].activo && choca(jugador.x+4, jugador.y+4, 24, 24, enemigos[i].x+4, enemigos[i].y+4, 24, 24)) {
                        enemigos[i].activo = FALSE; SPR_setVisibility(enemigos[i].spr, HIDDEN);
                        crearExp(enemigos[i].x, enemigos[i].y); sfxBoom();
                        if (vidas > 0) { vidas--; }
                        verHUD();
                        if (vidas == 0) {
                            gameOver = TRUE; SPR_setVisibility(jugador.spr, HIDDEN);
                            crearExp(jugador.x, jugador.y);
                            VDP_drawText("G A M E   O V E R", 11, 12);
                            VDP_drawText("PULSA START", 14, 14);
                        }
                    }
                }
            }
        }
        SPR_update(); SYS_doVBlankProcess();
    }
    return 0;
}
