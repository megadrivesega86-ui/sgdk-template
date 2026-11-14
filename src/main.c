#include <genesis.h>

int main() {
    VDP_drawText("Mi nuevo juego!", 10, 12);

    while(1) {
        SYS_doVBlankProcess();
    }
    return 0;
}
