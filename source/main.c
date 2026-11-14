#include <3ds.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *title;
    const char *system;
    const char *genre;
    const char *year;
    const char *description;
    int platform; /* 0 = DS, 1 = 3DS, 2 = spin-off */
} PokemonGame;

static const PokemonGame games[] = {
    {"HeartGold", "Nintendo DS", "Abenteuer", "2010", "Johto neu erleben: Reise durch Johto und Kanto.", 0},
    {"SoulSilver", "Nintendo DS", "Abenteuer", "2010", "Das Gegenstueck zu HeartGold mit eigener Legendenreise.", 0},
    {"Platin", "Nintendo DS", "Abenteuer", "2009", "Sinnoh mit der Zerrwelt und Giratina entdecken.", 0},
    {"Schwarz", "Nintendo DS", "Abenteuer", "2011", "Eine neue Region und eine neue Pokedex-Reise.", 0},
    {"Weiss", "Nintendo DS", "Abenteuer", "2011", "Die Einall-Region aus einer anderen Perspektive.", 0},
    {"Schwarz 2", "Nintendo DS", "Abenteuer", "2012", "Zwei Jahre spaeter geht das Einall-Abenteuer weiter.", 0},
    {"Weiss 2", "Nintendo DS", "Abenteuer", "2012", "Eine zweite Reise durch Einall mit neuen Orten.", 0},
    {"X", "Nintendo 3DS", "Abenteuer", "2013", "Kalos, Mega-Entwicklung und der 3D-Einstieg.", 1},
    {"Y", "Nintendo 3DS", "Abenteuer", "2013", "Erkunde Kalos und entdecke seltene Pokemon.", 1},
    {"Omega Rubin", "Nintendo 3DS", "Abenteuer", "2014", "Hoenn-Remake mit neuen Mega-Entwicklungen.", 1},
    {"Alpha Saphir", "Nintendo 3DS", "Abenteuer", "2014", "Hoenn aus einer zweiten Perspektive erleben.", 1},
    {"Sonne", "Nintendo 3DS", "Abenteuer", "2016", "Inselwanderschaft in der Alola-Region.", 1},
    {"Mond", "Nintendo 3DS", "Abenteuer", "2016", "Alola mit einer besonderen Tageszeit-Umkehr.", 1},
    {"Ultrasonne", "Nintendo 3DS", "Abenteuer", "2017", "Eine erweiterte Reise durch Alola.", 1},
    {"Ultramond", "Nintendo 3DS", "Abenteuer", "2017", "Neue Abenteuer und Geheimnisse in Alola.", 1},
    {"Pokemon Radical Red", "GBA-ROM-Hack", "ROM-Hack", "2022", "Ein anspruchsvoller FireRed-Hack mit vielen modernen Kampf- und Teamfunktionen.", 3},
    {"Pokemon Unbound", "GBA-ROM-Hack", "ROM-Hack", "2021", "Ein umfangreiches Fan-Abenteuer in der Borrius-Region.", 3},
    {"Pokemon Gaia", "GBA-ROM-Hack", "ROM-Hack", "2018", "Ein grosses Abenteuer in der Orbtus-Region mit neuen Geschichten.", 3},
    {"Pokemon Glazed", "GBA-ROM-Hack", "ROM-Hack", "2012", "Ein Fan-Abenteuer mit mehreren Regionen und eigener Handlung.", 3},
    {"Pokemon Inclement Emerald", "GBA-ROM-Hack", "ROM-Hack", "2022", "Eine deutlich anspruchsvollere Version von Pokemon Smaragd.", 3},
    {"Mystery Dungeon: Erkundungsteam Himmel", "Nintendo DS", "Spin-off", "2009", "Ein Dungeon-Abenteuer, in dem du selbst Pokemon bist.", 2},
    {"Pokemon Ranger: Finsternis über Almia", "Nintendo DS", "Spin-off", "2008", "Als Ranger Pokemon beschützen und Missionen lösen.", 2},
    {"Super Mystery Dungeon", "Nintendo 3DS", "Spin-off", "2016", "Ein grosses Mystery-Dungeon-Abenteuer mit vielen Pokemon.", 2}
};
#define GAME_COUNT ((int)(sizeof(games) / sizeof(games[0])))
#define CATEGORY_COUNT 7

static const char *categories[CATEGORY_COUNT] = {
    "Alle Spiele", "Nintendo DS", "Nintendo 3DS", "Spin-offs", "ROM-Hacks", "Merkliste", "Info"
};

static PrintConsole top, bottom;
static int category = 0;
static int game_index = 0;
static int view = 0; /* 0 categories, 1 list, 2 details, 3 random pick */
static int selected_game = 0;
static bool favourites[GAME_COUNT] = {false};
static char status_line[96] = "A waehlt  |  X merkt ein Spiel vor";

static int visible_count(void) {
    int n = 0;
    for (int i = 0; i < GAME_COUNT; ++i) {
        bool show = category == 0 ||
            (category == 1 && games[i].platform == 0) ||
            (category == 2 && games[i].platform == 1) ||
            (category == 3 && games[i].platform == 2) ||
            (category == 4 && games[i].platform == 3) ||
            (category == 5 && favourites[i]);
        if (show) ++n;
    }
    return n;
}

static int game_at(int visible) {
    int n = 0;
    for (int i = 0; i < GAME_COUNT; ++i) {
        bool show = category == 0 ||
            (category == 1 && games[i].platform == 0) ||
            (category == 2 && games[i].platform == 1) ||
            (category == 3 && games[i].platform == 2) ||
            (category == 4 && favourites[i]);
        if (show) {
            if (n == visible) return i;
            ++n;
        }
    }
    return -1;
}

/* 3DS framebuffers are sideways in memory and use BGR byte order. */
static void pixel(u8 *fb, int x, int y, int height, u8 r, u8 g, u8 b) {
    if (!fb || x < 0 || x >= 400 || y < 0 || y >= height) return;
    size_t at = ((size_t)x * height + (height - 1 - y)) * 3;
    fb[at] = b;
    fb[at + 1] = g;
    fb[at + 2] = r;
}

static void fill_rect(u8 *fb, int height, int x, int y, int w, int h,
                      u8 r, u8 g, u8 b) {
    for (int yy = y; yy < y + h; ++yy)
        for (int xx = x; xx < x + w; ++xx)
            pixel(fb, xx, yy, height, r, g, b);
}

static void circle(u8 *fb, int height, int cx, int cy, int radius,
                   u8 r, u8 g, u8 b) {
    int rr = radius * radius;
    for (int y = cy - radius; y <= cy + radius; ++y)
        for (int x = cx - radius; x <= cx + radius; ++x) {
            int dx = x - cx, dy = y - cy;
            if (dx * dx + dy * dy <= rr)
                pixel(fb, x, y, height, r, g, b);
        }
}

static void draw_orb(u8 *fb, int height, int cx, int cy, int radius) {
    circle(fb, height, cx + 3, cy + 5, radius + 2, 4, 7, 18);
    circle(fb, height, cx, cy, radius, 24, 31, 54);
    for (int y = cy - radius + 3; y < cy; ++y)
        for (int x = cx - radius; x <= cx + radius; ++x) {
            int dx = x - cx, dy = y - cy;
            if (dx * dx + dy * dy <= radius * radius)
                pixel(fb, x, y, height, 235, 66, 83);
        }
    for (int y = cy; y < cy + radius - 2; ++y)
        for (int x = cx - radius; x <= cx + radius; ++x) {
            int dx = x - cx, dy = y - cy;
            if (dx * dx + dy * dy <= radius * radius)
                pixel(fb, x, y, height, 238, 244, 250);
        }
    fill_rect(fb, height, cx - radius + 1, cy - 4, radius * 2 - 2, 8, 18, 25, 43);
    circle(fb, height, cx, cy, radius / 3, 245, 248, 252);
    circle(fb, height, cx, cy, radius / 5, 38, 53, 75);
    circle(fb, height, cx - radius / 3, cy - radius / 2, radius / 8, 255, 255, 255);
}

static void draw_background(gfxScreen_t screen) {
    u16 width = 0, height = 0;
    u8 *fb = gfxGetFramebuffer(screen, GFX_LEFT, &width, &height);
    if (!fb) return;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int glow = (x * 18 / (width ? width : 1));
            u8 r = (u8)(8 + glow / 3);
            u8 g = (u8)(15 + glow / 2);
            u8 b = (u8)(37 + glow);
            pixel(fb, x, y, height, r, g, b);
        }
    }
    /* Soft decorative panels and light trails. */
    fill_rect(fb, height, 8, 8, width - 16, 3, 38, 190, 220);
    fill_rect(fb, height, 8, height - 12, width - 16, 2, 33, 83, 130);
    for (int i = 0; i < 7; ++i) {
        int x = 18 + i * 43;
        circle(fb, height, x, 215 - (i % 3) * 13, 2 + (i % 2), 65, 154, 190);
    }
    if (screen == GFX_TOP) {
        /* Large illustrated Poké Ball-style orb, drawn from pixels, not text glyphs. */
        draw_orb(fb, height, 332, 122, 62);
        circle(fb, height, 282, 42, 5, 69, 180, 205);
        circle(fb, height, 380, 44, 3, 244, 180, 93);
        circle(fb, height, 284, 202, 4, 232, 95, 120);
        fill_rect(fb, height, 268, 204, 54, 2, 39, 105, 145);
    } else {
        draw_orb(fb, height, 272, 126, 47);
        circle(fb, height, 226, 45, 4, 69, 180, 205);
        circle(fb, height, 304, 202, 3, 244, 180, 93);
        fill_rect(fb, height, 228, 194, 70, 2, 39, 105, 145);
    }
}

static void clear_screens(void) {
    consoleSelect(&top);
    printf("\x1b[2J");
    consoleSelect(&bottom);
    printf("\x1b[2J");
}

static void open_project_page(void) {
    const char *url = "https://github.com/";
    char buffer[0x1000] = {0};
    size_t len = strlen(url);
    memcpy(buffer, url, len);
    aptLaunchSystemApplet(APPID_WEB, buffer, len + 1, 0);
}

static void draw_top(void) {
    consoleSelect(&top);
    printf("\x1b[1;1H");
    printf("\x1b[36mPOKEMON HUB\x1b[0m  \x1b[90mTOASTLINUS\x1b[0m\n");
    printf("\x1b[37mDeine Spiele. Deine naechste Reise.\x1b[0m\n\n");

    if (view == 0) {
        printf("\x1b[36mBIBLIOTHEK\x1b[0m\n\n");
        for (int i = 0; i < CATEGORY_COUNT; ++i) {
            if (i == category)
                printf("\x1b[46;30m > %-25.25s \x1b[0m\n", categories[i]);
            else
                printf("\x1b[37m   %-25.25s\x1b[0m\n", categories[i]);
        }
        printf("\n\x1b[90mA: Bereich oeffnen\x1b[0m\n");
    } else if (view == 1) {
        int count = visible_count();
        printf("\x1b[36m%s  (%d)\x1b[0m\n\n", categories[category], count);
        int first = game_index > 8 ? game_index - 8 : 0;
        int last = first + 9 < count ? first + 9 : count;
        for (int i = first; i < last; ++i) {
            int idx = game_at(i);
            if (idx < 0) continue;
            if (i == game_index)
                printf("\x1b[46;30m > %-25.25s \x1b[0m\n", games[idx].title);
            else
                printf("\x1b[37m   %-25.25s\x1b[0m\n", games[idx].title);
        }
        printf("\n\x1b[90mA: Details  X: Merken\x1b[0m\n");
    } else if (view == 2) {
        const PokemonGame *g = &games[selected_game];
        printf("\x1b[36mSPIELKARTE\x1b[0m\n\n");
        printf("\x1b[37m%s\x1b[0m\n\n", g->title);
        printf("\x1b[90mSystem: \x1b[37m%s\x1b[0m\n", g->system);
        printf("\x1b[90mJahr:   \x1b[37m%s\x1b[0m\n", g->year);
        printf("\x1b[90mGenre:  \x1b[37m%s\x1b[0m\n\n", g->genre);
        printf("\x1b[37m%s\x1b[0m\n\n", g->description);
        printf("%s\n", favourites[selected_game] ? "\x1b[32mIn deiner Merkliste\x1b[0m" : "\x1b[90mNoch nicht vorgemerkt\x1b[0m");
        printf("\x1b[90mX: Merkliste  B: Zurueck\x1b[0m\n");
    } else if (view == 3) {
        printf("\x1b[36mDEIN ZUFALLSTIPP\x1b[0m\n\n");
        printf("\x1b[37m%s\x1b[0m\n\n", games[selected_game].title);
        printf("\x1b[90m%s  |  %s\x1b[0m\n\n", games[selected_game].system, games[selected_game].year);
        printf("\x1b[37mVielleicht ist das dein naechstes Abenteuer!\x1b[0m\n\n");
        printf("\x1b[90mA: Spielkarte  Y: Neuer Tipp\x1b[0m\n");
    }
}

static int visible_favourite_count(void);

static void draw_bottom(void) {
    consoleSelect(&bottom);
    printf("\x1b[1;1H");
    printf("\x1b[36mDEIN POKEMON-ARCHIV\x1b[0m\n\n");
    if (view == 0) {
        printf("\x1b[37mWaehle, worauf du Lust hast.\x1b[0m\n\n");
        printf("\x1b[90mA  Bereich oeffnen\n");
        printf("Y  Zufallsspiel\n");
        printf("START  Beenden\x1b[0m\n\n");
        printf("\x1b[36mMERKLISTE\x1b[0m\n");
        printf("\x1b[37m%d vorgemerkte Spiele\x1b[0m\n", visible_favourite_count());
    } else if (view == 1) {
        int idx = game_at(game_index);
        if (idx >= 0) {
            printf("\x1b[37m%s\x1b[0m\n\n", games[idx].system);
            printf("\x1b[90m%s  |  %s\x1b[0m\n\n", games[idx].genre, games[idx].year);
            printf("\x1b[37mA: Spielkarte\nX: Merkliste\nY: Zufallstipp\nB: Zurueck\x1b[0m\n");
        } else {
            printf("\x1b[90mHier ist noch nichts vorgemerkt.\x1b[0m\n");
        }
    } else if (view == 2) {
        printf("\x1b[37m%s\x1b[0m\n\n", games[selected_game].description);
        printf("\x1b[90mDer Hub verteilt keine ROMs.\n");
        printf("Er hilft dir beim Entdecken\nund Auswaehlen deiner Spiele.\x1b[0m\n\n");
        printf("\x1b[36mX  Merkliste umschalten\x1b[0m\n");
        printf("\x1b[36mB  Zurueck zur Liste\x1b[0m\n");
    } else {
        printf("\x1b[37m%s\x1b[0m\n\n", games[selected_game].description);
        printf("\x1b[90mY waehlt ein anderes Spiel.\nA zeigt die Spielkarte.\nB geht zur Bibliothek.\x1b[0m\n");
    }
    printf("\n\x1b[90m%s\x1b[0m\n", status_line);
}

static int visible_favourite_count(void) {
    int n = 0;
    for (int i = 0; i < GAME_COUNT; ++i) if (favourites[i]) ++n;
    return n;
}

static void draw_all(void) {
    clear_screens();
    draw_background(GFX_TOP);
    draw_background(GFX_BOTTOM);
    draw_top();
    draw_bottom();
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}

static void pick_random(void) {
    selected_game = (int)(svcGetSystemTick() % GAME_COUNT);
    view = 3;
    snprintf(status_line, sizeof(status_line), "Zufallstipp: %s", games[selected_game].title);
}

int main(void) {
    gfxInitDefault();
    consoleInit(GFX_TOP, &top);
    consoleInit(GFX_BOTTOM, &bottom);
    draw_all();

    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();
        bool changed = false;

        if (view == 0) {
            if (down & KEY_UP) { category = (category + CATEGORY_COUNT - 1) % CATEGORY_COUNT; changed = true; }
            if (down & KEY_DOWN) { category = (category + 1) % CATEGORY_COUNT; changed = true; }
            if (down & KEY_A) {
                if (category == 6) {
                    snprintf(status_line, sizeof(status_line), "Pokemon Hub - Bibliothek und Entdecker");
                } else {
                    view = 1;
                    game_index = 0;
                }
                changed = true;
            }
            if (down & KEY_Y) { pick_random(); changed = true; }
            if (down & KEY_START) break;
        } else if (view == 1) {
            int count = visible_count();
            if (count > 0 && (down & KEY_UP)) { game_index = (game_index + count - 1) % count; changed = true; }
            if (count > 0 && (down & KEY_DOWN)) { game_index = (game_index + 1) % count; changed = true; }
            if (down & KEY_A && count > 0) {
                selected_game = game_at(game_index);
                view = 2;
                changed = true;
            }
            if (down & KEY_X && count > 0) {
                int idx = game_at(game_index);
                if (idx >= 0) {
                    favourites[idx] = !favourites[idx];
                    snprintf(status_line, sizeof(status_line), "%s %s", games[idx].title, favourites[idx] ? "gemerkt" : "entfernt");
                }
                changed = true;
            }
            if (down & KEY_Y) { pick_random(); changed = true; }
            if (down & KEY_B) { view = 0; changed = true; }
        } else if (view == 2) {
            if (down & KEY_X) {
                favourites[selected_game] = !favourites[selected_game];
                snprintf(status_line, sizeof(status_line), "%s %s", games[selected_game].title, favourites[selected_game] ? "gemerkt" : "entfernt");
                changed = true;
            }
            if (down & KEY_B) { view = 1; changed = true; }
        } else {
            if (down & KEY_A) { view = 2; changed = true; }
            if (down & KEY_Y) { pick_random(); changed = true; }
            if (down & KEY_B) { view = 0; changed = true; }
        }

        if (changed) draw_all();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
