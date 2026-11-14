#include <3ds.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *title;
    const char *system;
    const char *description;
} ShopItem;

static const ShopItem items[] = {
    {"Community projects", "DS + 3DS", "Browse creator pages and authorized releases."},
    {"New Super Mario Bros. DS", "Nintendo DS", "Fan-made levels, packs and ROM hacks."},
    {"New Super Mario Bros. 2", "Nintendo 3DS", "Community mods and custom content."},
    {"How downloads work", "INFO", "Use creator-authorized patches with your own game copy."}
};

static void draw_screen(PrintConsole *screen, const char *heading, int selected) {
    consoleSelect(screen);
    printf("\x1b[2J");
    printf("\x1b[1;1H");
    printf("\x1b[36m================================\x1b[0m\n");
    printf("\x1b[1;3H NSMBHD SHOP\n");
    printf("\x1b[36m================================\x1b[0m\n\n");
    printf("%s\n\n", heading);
    for (int i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
        if (i == selected) printf("\x1b[47;30m > %-27s \x1b[0m\n", items[i].title);
        else printf("   %-27s\n", items[i].title);
    }
}

int main(void) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    PrintConsole bottom;
    consoleInit(GFX_BOTTOM, &bottom);

    int selected = 0;
    const int count = sizeof(items) / sizeof(items[0]);
    bool running = true;

    while (aptMainLoop() && running) {
        hidScanInput();
        u32 down = hidKeysDown();

        if (down & KEY_UP) selected = (selected + count - 1) % count;
        if (down & KEY_DOWN) selected = (selected + 1) % count;
        if (down & KEY_B) running = false;

        draw_screen(gfxGetConsole(GFX_TOP), "DISCOVER MARIO MODS", selected);
        consoleSelect(&bottom);
        printf("\x1b[2J");
        printf("\x1b[1;1H\x1b[36mPROJECT DETAILS\x1b[0m\n\n");
        printf("%s\n", items[selected].system);
        printf("------------------------------\n");
        printf("%s\n\n", items[selected].description);
        printf("UP/DOWN  Select category\n");
        printf("A        Open selection\n");
        printf("B        Exit\n\n");
        printf("\x1b[33mShop preview build 0.1\x1b[0m\n");
        if (down & KEY_A) {
            printf("\nSelected: %s\n", items[selected].title);
            printf("Catalog and downloads are being wired up.\n");
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
