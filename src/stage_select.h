#ifndef STAGE_SELECT_H
#define STAGE_SELECT_H
#include <genesis.h>
static const char* opFaseTxt[7] = {
    "1. FASE 1 - ORBITA",
    "2. JEFE FASE 1",
    "3. FASE 2 - ATMOSFERA",
    "4. JEFE FASE 2",
    "5. FASE 3 - ESPACIO PROFUNDO",
    "6. JEFE FASE 3",
    "7. CREDITOS FINALES"
};
static u8 elegirFaseTemporal() {
    u8 sel = 0; u16 prev = JOY_readJoypad(JOY_1);
    VDP_clearPlane(BG_A, TRUE); VDP_clearPlane(BG_B, TRUE);
    VDP_setTextPlane(BG_A); VDP_setTextPalette(PAL3);
    VDP_drawText("=== STAGE SELECT (TEMP) ===", 6, 4);
    VDP_drawText("ARRIBA/ABAJO + START", 9, 24);
    while (1) {
        u16 m = JOY_readJoypad(JOY_1);
        if ((m & BUTTON_UP) && !(prev & BUTTON_UP)) { VDP_drawText("  ", 4, 8 + sel * 2); sel = (sel == 0) ? 6 : sel - 1; }
        if ((m & BUTTON_DOWN) && !(prev & BUTTON_DOWN)) { VDP_drawText("  ", 4, 8 + sel * 2); sel = (sel == 6) ? 0 : sel + 1; }
        for (u8 i = 0; i < 7; i++) VDP_drawText(opFaseTxt[i], 7, 8 + i * 2);
        VDP_drawText("->", 4, 8 + sel * 2);
        if ((m & (BUTTON_START | BUTTON_A | BUTTON_B | BUTTON_C)) && !(prev & (BUTTON_START | BUTTON_A | BUTTON_B | BUTTON_C))) {
            VDP_clearPlane(BG_A, TRUE); return sel;
        }
        prev = m; SYS_doVBlankProcess();
    }
}
#endif
