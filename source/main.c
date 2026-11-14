#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define ITEM_COUNT 6

typedef struct {
    const char *title;
    const char *tag;
    const char *description;
    const char *detail;
} ShopItem;

static const ShopItem items[ITEM_COUNT] = {
    {"Featured projects", "FEATURED", "Hand-picked community projects.", "A curated front page for interesting New Super Mario Bros. community projects.\nCreator pages and authorized downloads will appear here."},
    {"NSMB DS", "NINTENDO DS", "Custom levels, packs and fan projects.", "Explore projects for New Super Mario Bros. on Nintendo DS.\nThe shop will link to creator-approved pages and patches."},
    {"NSMB 2", "NINTENDO 3DS", "Custom content for the 3DS game.", "Discover community-made content for New Super Mario Bros. 2.\nOnly creator-authorized downloads and patch information belong here."},
    {"Latest updates", "WHAT'S NEW", "See recently updated projects.", "A future update feed will show new versions, changelogs\nand announcements from the project creators."},
    {"My favorites", "SAVED", "Your bookmarked community projects.", "Your personal project list will live here.\nFavorite support is planned for a later version."},
    {"About & safety", "INFORMATION", "How the community shop works.", "NSMBHD Shop is a fan-made project browser.\nUse patches only with a game copy you own.\nDownloads must be shared with the creator's permission."}
};

static int selected = 0;
static int detail_page = 0;
static PrintConsole top;
static PrintConsole bottom;

static void top_screen(void) {
    consoleSelect(&top);
    printf("\x1b[2J\x1b[H");
    printf("\x1b[46;30m                                                  \x1b[0m\n");
    printf("\x1b[46;30m   * NSMBHD SHOP                         v0.2     \x1b[0m\n");
    printf("\x1b[46;30m                                                  \x1b[0m\n");
    printf("\x1b[36m  COMMUNITY MODS  \x1b[90m/  DISCOVER                         \x1b[0m\n\n");
    printf("\x1b[37m  Welcome to the community hub!\x1b[0m\n");
    printf("\x1b[90m  Choose a section to explore.\x1b[0m\n\n");

    for (int i = 0; i < ITEM_COUNT; i++) {
        if (i == selected) {
            printf("\x1b[46;30m  > %-42s\x1b[0m\n", items[i].title);
            printf("\x1b[36m    %-44s\x1b[0m\n", items[i].description);
        } else {
            printf("\x1b[90m    %-44s\x1b[0m\n", items[i].title);
            printf("    \x1b[90m%-44s\x1b[0m\n", items[i].description);
        }
        printf("\n");
    }

    printf("\x1b[90m  ----------------------------------------------\x1b[0m\n");
    printf("\x1b[32m  READY\x1b[0m  \x1b[90mCommunity browser preview\x1b[0m\n");
}

static void bottom_screen(void) {
    consoleSelect(&bottom);
    printf("\x1b[2J\x1b[H");
    printf("\x1b[44;37m                                                  \x1b[0m\n");
    if (detail_page) {
        printf("\x1b[44;37m   PROJECT INFORMATION                            \x1b[0m\n");
    } else {
        printf("\x1b[44;37m   QUICK VIEW                                     \x1b[0m\n");
    }
    printf("\x1b[44;37m                                                  \x1b[0m\n\n");

    printf("\x1b[36m  %s\x1b[0m\n", items[selected].tag);
    printf("\x1b[37m  %s\x1b[0m\n\n", items[selected].title);
    printf("\x1b[90m  ----------------------------------------------\x1b[0m\n");

    if (detail_page) {
        printf("\x1b[37m  %s\x1b[0m\n\n", items[selected].detail);
        printf("\x1b[33m  THIS IS A PREVIEW BUILD\x1b[0m\n");
        printf("\x1b[90m  Project catalog and creator links are coming.\x1b[0m\n\n");
        printf("\x1b[36m  B\x1b[0m  Back to sections\n");
    } else {
        printf("\x1b[37m  %s\x1b[0m\n\n", items[selected].description);
        printf("\x1b[90m  Select a section and press A to\n");
        printf("  see its information.\x1b[0m\n\n");
        printf("\x1b[36m  UP / DOWN\x1b[0m  Move through sections\n");
        printf("\x1b[36m  A\x1b[0m          Open section\n");
        printf("\x1b[36m  B\x1b[0m          Exit shop\n");
    }

    printf("\n\x1b[90m  NSMBHD SHOP  |  MADE BY TOASTLINUS\x1b[0m\n");
}

static void draw_all(void) {
    top_screen();
    bottom_screen();
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}

int main(void) {
    gfxInitDefault();
    consoleInit(GFX_TOP, &top);
    consoleInit(GFX_BOTTOM, &bottom);

    bool running = true;
    draw_all();

    while (aptMainLoop() && running) {
        hidScanInput();
        u32 down = hidKeysDown();
        bool changed = false;

        if (!detail_page) {
            if (down & KEY_UP) {
                selected = (selected + ITEM_COUNT - 1) % ITEM_COUNT;
                changed = true;
            }
            if (down & KEY_DOWN) {
                selected = (selected + 1) % ITEM_COUNT;
                changed = true;
            }
            if (down & KEY_A) {
                detail_page = 1;
                changed = true;
            }
            if (down & KEY_B) running = false;
        } else {
            if (down & KEY_B) {
                detail_page = 0;
                changed = true;
            }
            if (down & KEY_LEFT) {
                selected = (selected + ITEM_COUNT - 1) % ITEM_COUNT;
                changed = true;
            }
            if (down & KEY_RIGHT) {
                selected = (selected + 1) % ITEM_COUNT;
                changed = true;
            }
        }

        /* Redraw only after an input changes the page or selection.
           This avoids clearing both LCDs every frame, which caused flicker. */
        if (changed) draw_all();

        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
