#include <genesis.h>
#include "resources.h"

#define MAX_BALAS 16
#define MAX_ENEMIGOS 6
#define MAX_EXP 4
#define ID_SFX_LASER 64
#define ID_SFX_BOOM  65
#define ID_BGM_TEMA  66
#define ID_SFX_START 67
#define ID_BGM_SEL   68

typedef struct { Sprite* spr; s16 x, y, vx; bool activo; u16 vida; } Entidad;
Entidad pwr;
u8 nivelArma = 1, bajasPwr = 0, tipoPwr = 0, faseActual = 0;
u16 avisoOrbita = 0, despegue = 0;
u8 escudo = 0;
Sprite* sprEscudo;
Entidad jugador, balas[MAX_BALAS], enemigos[MAX_ENEMIGOS], exps[MAX_EXP];
u32 puntos = 0;
u16 vidas = 3, cd = 0, timer = 0, prevM = 0;
u8 pausado = 0, estadoJuego = 0;
u8 naveElegida = 0;
bool autoFire = FALSE;
s16 scrollY = 0;
char hud[40];

void sfxLaser() { XGM_startPlayPCM(ID_SFX_LASER, 10, SOUND_PCM_CH2); }
void sfxBoom()  { XGM_startPlayPCM(ID_SFX_BOOM, 14, SOUND_PCM_CH3); }
void sfxStart() { XGM_startPlayPCM(ID_SFX_START, 15, SOUND_PCM_CH4); }

void verHUD() {
    static u8 tileIcono = 0;
    if (!tileIcono) {
        const u32 ico[8] = {0x000FF000,0x00FFF000,0x0FFFFF00,0xFF0F0FF0,0xFFFFF0FF,0xF0FFF00F,0x00F0F000,0x00000000};
        VDP_loadTileData(ico, 1400, 1, CPU);
        tileIcono = 1;
    }
    char h1[42], h2[42], h3[42];
    const char* nom[3] = {"DREAMER", "RAVEN-X", "COLOSSUS"};
    PAL_setColor(63, RGB24_TO_VDPCOLOR(0xFFCC00));
    VDP_setTextPalette(PAL3);
    sprintf(h1, "1UP           SCORE     STAGE %d", faseActual + 1);
    VDP_drawText(h1, 1, 1);
    VDP_setTextPalette(PAL0);
    sprintf(h2, "%-8s      %06lu           ", nom[naveElegida], puntos);
    VDP_drawText(h2, 1, 2);
    sprintf(h3, "  x%-2d              %4s %7s", vidas,
        (escudo == 3) ? "[S3]" : ((escudo == 2) ? "[S2]" : ((escudo == 1) ? "[S1]" : "    ")),
        (nivelArma > 1) ? "[W-MAX]" : (autoFire ? "[A]    " : "       "));
    VDP_drawText(h3, 1, 3); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, 1400), 1, 3); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, 1400), 2, 3);
}

void crearExp(s16 x, s16 y) {
    for (int i = 0; i < MAX_EXP; i++) {
        if (!exps[i].activo) {
            exps[i].activo = TRUE; exps[i].x = x; exps[i].y = y; exps[i].vida = 12;
            SPR_setVisibility(exps[i].spr, VISIBLE); SPR_setPosition(exps[i].spr, x, y); break;
        }
    }
}

void lanzarBala(s16 bx, s16 by, s16 bvx) {
    for (int i = 0; i < MAX_BALAS; i++) {
        if (!balas[i].activo) {
            balas[i].activo = TRUE; balas[i].x = bx; balas[i].y = by; balas[i].vx = bvx;
            SPR_setVisibility(balas[i].spr, VISIBLE); SPR_setPosition(balas[i].spr, bx, by); break;
        }
    }
}

void disparar() {
    if (naveElegida == 2) {
        lanzarBala(jugador.x + 4, jugador.y - 4, 0);
        lanzarBala(jugador.x + 20, jugador.y - 4, 0);
    } else {
        lanzarBala(jugador.x + 12, jugador.y - 6, 0);
    }
    if (nivelArma >= 2) {
        lanzarBala(jugador.x + 2, jugador.y, -2);
        lanzarBala(jugador.x + 22, jugador.y, 2);
    }
    sfxLaser();
}

extern u8 jefeAct, jefeMuriendo, jefeIntro, jefeCine;
void crearEnemigo() {
    if (!jefeAct && !jefeMuriendo && !jefeIntro && !jefeCine) for (int i = 0; i < MAX_ENEMIGOS; i++) {
        if (!enemigos[i].activo) {
            enemigos[i].activo = TRUE; enemigos[i].x = (random() % 272) + 16; enemigos[i].y = -32;
            SPR_setVisibility(enemigos[i].spr, VISIBLE); SPR_setPosition(enemigos[i].spr, enemigos[i].x, enemigos[i].y); break;
        }
    }
}

bool choca(s16 x1, s16 y1, s16 w1, s16 h1, s16 x2, s16 y2, s16 w2, s16 h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}
void dibujarBandas(const Image* top, const Image* mid, const Image* bot) {
    SPR_reset();
    VDP_setVerticalScroll(BG_B, 0);
    PAL_setPalette(PAL0, top->palette->data, DMA);
    PAL_setPalette(PAL2, mid->palette->data, DMA);
    PAL_setPalette(PAL3, bot->palette->data, DMA);
    VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE);
    u16 idx = TILE_USER_INDEX;
    VDP_drawImageEx(BG_B, top, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, idx), 0, 0, FALSE, TRUE);
    idx += top->tileset->numTile;
    VDP_drawImageEx(BG_B, mid, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, idx), 0, 6, FALSE, TRUE);
    idx += mid->tileset->numTile;
    VDP_drawImageEx(BG_B, bot, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, idx), 0, 22, FALSE, TRUE);
}

void mostrarTitulo() {
    estadoJuego = 0; SPR_reset();
    VDP_setVerticalScroll(BG_B, 0);
    PAL_setPalette(PAL0, img_ptop.palette->data, DMA);
    PAL_setPalette(PAL2, img_pmid.palette->data, DMA);
    PAL_setPalette(PAL3, img_pbot.palette->data, DMA);
    VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE);
    u16 idx = TILE_USER_INDEX;
    VDP_drawImageEx(BG_B, &img_ptop, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, idx), 0, 0, FALSE, TRUE);
    idx += img_ptop.tileset->numTile;
    VDP_drawImageEx(BG_B, &img_pmid, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, idx), 0, 9, FALSE, TRUE);
    idx += img_pmid.tileset->numTile;
    VDP_drawImageEx(BG_B, &img_pbot, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, idx), 0, 23, FALSE, TRUE);
}

void mostrarSeleccion() {
    estadoJuego = 1;
    if (naveElegida == 0) dibujarBandas(&img_s1_top, &img_s1_mid, &img_s1_bot);
    else if (naveElegida == 1) dibujarBandas(&img_s2_top, &img_s2_mid, &img_s2_bot);
    else dibujarBandas(&img_s3_top, &img_s3_mid, &img_s3_bot);
    PAL_setColor(63, RGB24_TO_VDPCOLOR(0xFFFFFF));
    VDP_setTextPalette(PAL3);
    if (naveElegida == 0) {
        VDP_drawText("SPD: ===--  VIDAS: 3", 9, 23);
        VDP_drawText("ARMA: LASER CENTRAL ", 9, 24);
    } else if (naveElegida == 1) {
        VDP_drawText("SPD: =====  VIDAS: 2", 9, 23);
        VDP_drawText("ARMA: DISPARO RAPIDO", 9, 24);
    } else {
        VDP_drawText("SPD: ==---  VIDAS: 5", 9, 23);
        VDP_drawText("ARMA: DOBLE PARALELO", 9, 24);
    }
    VDP_drawText("< IZQ/DER >  START:JUGAR", 9, 26);
}

#include "jefe_10k.h"
void empezarPartida() { resetJefe10k();
    puntos = 0; estadoJuego = 2; nivelArma = 1; bajasPwr = 0; escudo = 0;
    vidas = (naveElegida == 1) ? 2 : ((naveElegida == 2) ? 5 : 3);
    SPR_reset();
    VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE);
    faseActual = 0; avisoOrbita = 0;
    PAL_setPalette(PAL0, img_tierra.palette->data, DMA);
    PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF));
    PAL_setPalette(PAL2, spr_enemigo.palette->data, DMA);
    PAL_setPalette(PAL3, spr_pwr.palette->data, DMA);
    VDP_drawImageEx(BG_B, &img_tierra, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX), 0, 0, FALSE, TRUE);
    VDP_drawImageEx(BG_B, &img_tierra, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX), 32, 0, FALSE, TRUE);

    const SpriteDefinition* defNave = (naveElegida == 1) ? &spr_nave_r : ((naveElegida == 2) ? &spr_nave_a : &spr_nave);
    PAL_setPalette(PAL1, defNave->palette->data, DMA);
    jugador.x = 144; jugador.y = 216; despegue = 300;
    VDP_setTextPalette(PAL0); VDP_drawText("DESPEGANDO...", 13, 12);
    jugador.spr = SPR_addSprite(defNave, jugador.x, jugador.y, TILE_ATTR(PAL1, 0, FALSE, FALSE));

    for (int i = 0; i < MAX_BALAS; i++) { balas[i].activo = FALSE; balas[i].spr = SPR_addSprite(&spr_laser, 0, 0, TILE_ATTR(PAL3, 0, FALSE, FALSE)); SPR_setVisibility(balas[i].spr, HIDDEN); }
    if (!jefeAct && !jefeMuriendo && !jefeIntro && !jefeCine) for (int i = 0; i < MAX_ENEMIGOS; i++) { enemigos[i].activo = FALSE; enemigos[i].spr = SPR_addSprite(&spr_enemigo, 0, 0, TILE_ATTR(PAL2, 0, FALSE, FALSE)); SPR_setVisibility(enemigos[i].spr, HIDDEN); }
    for (int i = 0; i < MAX_EXP; i++) { exps[i].activo = FALSE; exps[i].spr = SPR_addSprite(&spr_exp, 0, 0, TILE_ATTR(PAL3, 0, FALSE, FALSE)); SPR_setVisibility(exps[i].spr, HIDDEN); }
    pwr.activo = FALSE; pwr.spr = SPR_addSprite(&spr_pwr, 0, 0, TILE_ATTR(PAL3, 0, FALSE, FALSE)); SPR_setVisibility(pwr.spr, HIDDEN);
    sprEscudo = SPR_addSprite(&spr_escudo, jugador.x, jugador.y, TILE_ATTR(PAL3, 0, FALSE, FALSE)); SPR_setVisibility(sprEscudo, HIDDEN);
    verHUD();
}
int main() {
    JOY_init(); SPR_init();
    XGM_setPCM(ID_SFX_LASER, sfx_laser, sizeof(sfx_laser));
    XGM_setPCM(ID_SFX_BOOM, sfx_boom, sizeof(sfx_boom));
    
    XGM_setPCM(ID_SFX_START, sfx_start, sizeof(sfx_start));
    
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    mostrarTitulo();
    XGM_startPlay(mus_titulo);

    while(1) {
        u16 m = JOY_readJoypad(JOY_1); if (jefeCine > 0) m = 0;
        if (estadoJuego == 0) {
            if (++timer >= 50) { timer = 0; }
            if (timer == 1) { PAL_setPalette(PAL3, img_pbot.palette->data, DMA); }
            else if (timer == 26) {
                for (u16 k = 1; k < 16; k++) {
                    u16 col = img_pbot.palette->data[k];
                    u16 r = col & 0x000E; u16 b = (col >> 8) & 0x000E;
                    if (r >= b) { PAL_setColor(48 + k, RGB24_TO_VDPCOLOR(0x040818)); }
                }
            }
            if ((m & BUTTON_START) && !(prevM & BUTTON_START)) { timer = 0; sfxStart();  XGM_startPlay(mus_hangar); mostrarSeleccion(); }
        SPR_update(); SYS_doVBlankProcess(); continue;
        }
        if (estadoJuego == 1) {
            if ((m & BUTTON_LEFT) && !(prevM & BUTTON_LEFT)) { naveElegida = (naveElegida == 0) ? 2 : naveElegida - 1; sfxLaser(); mostrarSeleccion(); }
            if ((m & BUTTON_RIGHT) && !(prevM & BUTTON_RIGHT)) { naveElegida = (naveElegida + 1) % 3; sfxLaser(); mostrarSeleccion(); }
            if ((m & (BUTTON_START | BUTTON_A | BUTTON_C)) && !(prevM & (BUTTON_START | BUTTON_A | BUTTON_C))) { sfxStart(); XGM_startPlay(mus_juego); empezarPartida(); }
            prevM = m; SPR_update(); SYS_doVBlankProcess(); continue;
        }
        if (estadoJuego == 3) {
            if ((m & BUTTON_START) && !(prevM & BUTTON_START)) { mostrarTitulo();
    XGM_startPlay(mus_titulo); }
            prevM = m; SPR_update(); SYS_doVBlankProcess(); continue;
        }
        if (despegue > 0) {
            despegue--;
            s16 velScroll = (despegue > 200) ? 1 : ((despegue > 100) ? 2 : 3);
            scrollY -= velScroll;
            VDP_setVerticalScroll(BG_B, scrollY);
            if (jugador.y > 170 && (despegue % 4 == 0)) { jugador.y--; }
            SPR_setAnim(jugador.spr, (m & (BUTTON_LEFT | BUTTON_RIGHT)) ? 1 : 0);
        SPR_setHFlip(jugador.spr, (m & BUTTON_RIGHT) ? TRUE : FALSE);
        SPR_setPosition(jugador.spr, jugador.x, jugador.y);
            if (despegue == 0) { VDP_clearText(13, 12, 14); }
            prevM = m; SPR_update(); SYS_doVBlankProcess(); continue;
        }
        if (vidas > 0 && despegue == 0 && !jefeCine && (m & BUTTON_START) && !(prevM & BUTTON_START)) {
            pausado = !pausado; sfxStart();
            if (pausado) { XGM_pausePlay(); VDP_setTextPalette(PAL0); VDP_drawText("P A U S A", 15, 13); }
            else { VDP_clearText(14, 13, 12); VDP_clearText(14, 14, 12); XGM_resumePlay(); }
        }
        if (pausado) { prevM = m; SYS_doVBlankProcess(); continue; }
        scrollY -= 3; VDP_setVerticalScroll(BG_B, scrollY);
        if (avisoOrbita > 0 && --avisoOrbita == 0) { VDP_clearText(11, 12, 18); }
        s16 vel = (naveElegida == 1) ? 5 : ((naveElegida == 2) ? 2 : 3);
        u16 recarga = (naveElegida == 1) ? 4 : ((naveElegida == 2) ? 10 : 7);
        if (m & BUTTON_LEFT) { jugador.x -= vel; } if (m & BUTTON_RIGHT) { jugador.x += vel; }
        if (m & BUTTON_UP) { jugador.y -= vel; } if (m & BUTTON_DOWN) { jugador.y += vel; }
        if (jugador.x < 8) { jugador.x = 8; } if (jugador.x > 280) { jugador.x = 280; }
        if (!jefeCine && jugador.y < 24) { jugador.y = 24; } if (jugador.y > 184) { jugador.y = 184; }
        SPR_setAnim(jugador.spr, (m & (BUTTON_LEFT | BUTTON_RIGHT)) ? 1 : 0);
        SPR_setHFlip(jugador.spr, (m & BUTTON_RIGHT) ? TRUE : FALSE);
        SPR_setPosition(jugador.spr, jugador.x, jugador.y);
        if (escudo > 0) { SPR_setPosition(sprEscudo, jugador.x, jugador.y); SPR_setVisibility(sprEscudo, (escudo == 1 && (timer & 2)) ? HIDDEN : VISIBLE); } else { SPR_setVisibility(sprEscudo, HIDDEN); }
        if ((m & BUTTON_B) && !(prevM & BUTTON_B)) { autoFire = !autoFire; verHUD(); }
        prevM = m;
        if (cd > 0) { cd--; }
        if (((m & (BUTTON_A | BUTTON_C)) || autoFire) && cd == 0) { disparar(); cd = recarga; }
        for (int i = 0; i < MAX_BALAS; i++) if (balas[i].activo) {
            balas[i].y -= 6; balas[i].x += balas[i].vx;
            if (balas[i].y < -8 || balas[i].x < -8 || balas[i].x > 328) { balas[i].activo = FALSE; SPR_setVisibility(balas[i].spr, HIDDEN); }
            else SPR_setPosition(balas[i].spr, balas[i].x, balas[i].y);
        }
        for (int i = 0; i < MAX_EXP; i++) if (exps[i].activo) {
            if (--exps[i].vida == 0) { exps[i].activo = FALSE; SPR_setVisibility(exps[i].spr, HIDDEN); }
        }
        if (pwr.activo) {
            pwr.y += 2;
            if (pwr.y > 224) { pwr.activo = FALSE; SPR_setVisibility(pwr.spr, HIDDEN); }
            else {
                SPR_setPosition(pwr.spr, pwr.x, pwr.y); SPR_setFrame(pwr.spr, tipoPwr * 6 + ((timer >> 2) % 6));
                if (choca(jugador.x, jugador.y, 32, 32, pwr.x, pwr.y, 24, 24)) {
                    pwr.activo = FALSE; SPR_setVisibility(pwr.spr, HIDDEN); sfxStart();
                    if (tipoPwr == 0) { if (nivelArma == 1) nivelArma = 2; else puntos += 300; }
                    else if (tipoPwr == 1) { if (escudo < 3) escudo = 3; else puntos += 300; }
                    else { if (vidas < 10) vidas++; puntos += 500; }
                    verHUD();
                }
            }
        }
        if (++timer >= 42) { crearEnemigo(); timer = 0; }
        if (!jefeAct && !jefeMuriendo && !jefeIntro && !jefeCine) for (int i = 0; i < MAX_ENEMIGOS; i++) if (enemigos[i].activo) {
            enemigos[i].y += (faseActual > 0) ? 3 : 2;
                if (faseActual > 0) {
                    enemigos[i].x += (((timer + i * 19) & 64) ? 1 : -1);
                    if (enemigos[i].x < 16) enemigos[i].x = 16;
                    if (enemigos[i].x > 272) enemigos[i].x = 272;
                }
            if (enemigos[i].y > 224) { enemigos[i].activo = FALSE; SPR_setVisibility(enemigos[i].spr, HIDDEN); }
            else {
                SPR_setPosition(enemigos[i].spr, enemigos[i].x, enemigos[i].y);
                for (int b = 0; b < MAX_BALAS; b++) if (balas[b].activo && choca(balas[b].x, balas[b].y, 8, 8, enemigos[i].x+2, enemigos[i].y+2, 28, 28)) {
                    balas[b].activo = FALSE; SPR_setVisibility(balas[b].spr, HIDDEN);
                    enemigos[i].activo = FALSE; SPR_setVisibility(enemigos[i].spr, HIDDEN);
                    crearExp(enemigos[i].x, enemigos[i].y); sfxBoom(); puntos += (faseActual > 0) ? 150 : 100; if (++bajasPwr >= 4 && !pwr.activo) {
                        bajasPwr = 0;
                        u8 disp[3], nd = 0;
                        if (nivelArma < 2) disp[nd++] = 0;
                        if (escudo == 0) disp[nd++] = 1;
                        if (vidas < 10) disp[nd++] = 2;
                        if (nd > 0) {
                            tipoPwr = disp[random() % nd];
                            pwr.activo = TRUE; SPR_setFrame(pwr.spr, tipoPwr * 6);
                            pwr.x = enemigos[i].x + 8; pwr.y = enemigos[i].y;
                            SPR_setVisibility(pwr.spr, VISIBLE); SPR_setPosition(pwr.spr, pwr.x, pwr.y); SPR_setFrame(pwr.spr, tipoPwr * 6 + ((timer >> 2) % 6));
                        } else { puntos += 500; }
                    } if (faseActual == 0 && puntos >= 10000) {
                        faseActual = 1; avisoOrbita = 90; sfxStart();
                        VDP_setTextPalette(PAL0);
                    }
                    verHUD(); break;
                }
                if (enemigos[i].activo && choca(jugador.x+4, jugador.y+4, 24, 24, enemigos[i].x+4, enemigos[i].y+4, 24, 24)) {
                    enemigos[i].activo = FALSE; SPR_setVisibility(enemigos[i].spr, HIDDEN);
                    crearExp(enemigos[i].x, enemigos[i].y); sfxBoom();
                    if (escudo > 0) { escudo--; if (escudo == 0) SPR_setVisibility(sprEscudo, HIDDEN); } else { nivelArma = 1; if (vidas > 0) { vidas--; } } verHUD();
                    if (vidas == 0) {
                        VDP_setTextPalette(PAL0);
                        VDP_drawText("G A M E   O V E R", 11, 12); if (vidas == 0) VDP_drawText("PULSA START", 15, 14);
                    }
                }
            }
        }
        if (estadoJuego == 2) cicloJefe10k(jugador.x, jugador.y);
        SPR_update(); SYS_doVBlankProcess();
    }
    return 0;
}
