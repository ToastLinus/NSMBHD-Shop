#include <3ds.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *title;
    const char *creator;
    const char *version;
    const char *status;
    const char *description;
    const char *url;
    int system;
    int kaizo;
} Project;

static const Project projects[] = {
    {"Newer Super Mario Bros. DS", "Newer Team", "Latest listed", "Finished", "A full New Super Mario Bros. DS community hack with new levels, graphics, enemies and more.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0, 0},
    {"New Super Mario Bros. Origins", "SKJmin", "1.0a rev2", "Finished", "A community-made New Super Mario Bros. DS hack. Check the project page for current files and notes.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0},
    {"New Super Luigi Bros. Atlantis Revolution 2.5", "Luigui3060", "2.5", "Finished", "A Luigi-focused DS hack listed in the NSMBHD project index.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0},
    {"New Super Mario Bros. U DS", "Skylander", "1.08 listed", "Finished", "A DS community project inspired by New Super Mario Bros. U.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0, 0},
    {"Kaizo 1", "Play Gamer", "Latest listed", "Finished (4 levels)", "A short, Kaizo-style New Super Mario Bros. DS hack with four listed levels.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0, 1},
    {"Kaizo Inspired Mario", "frjd", "Latest listed", "Cancelled (2 levels)", "A Kaizo-inspired New Super Mario Bros. DS project; the community list marks it cancelled after two levels.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0, 1},
    {"New Super Kazio Bros.", "Joe", "Beta", "Cancelled (W1 + 11 levels)", "A difficult New Super Mario Bros. DS project listed as a beta; the community index marks it cancelled.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0, 1},
    {"Newer Super Mario Bros. 2", "Aaron Mallie", "Latest listed", "Finished", "A full New Super Mario Bros. 2 project for Nintendo 3DS.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB2%29/", 1, 0},
    {"More Super Mario Bros. 3DS", "KarlMaa", "Demo", "Demo", "A New Super Mario Bros. 2 community project for Nintendo 3DS.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB2%29/", 1},
    {"New Super Mario Bros. 2 Anew!", "GreyMask", "1.2", "Demo", "A New Super Mario Bros. 2 hack with a first-world demo listed by the community wiki.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB2%29/", 1},
    {"New Super Mario Bros. 2 Mastermode!!", "NZia", "Latest listed", "Demo", "A challenge-focused NSMB2 project with a demo listing.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB2%29/", 1},
    {"New Super Mario Bros. 2 Take 2", "ThorgeLP", "Latest listed", "Demo", "A community project with a four-level demo listing.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB2%29/", 1}
};
#define PROJECT_COUNT (sizeof(projects) / sizeof(projects[0]))
#define CATEGORY_COUNT 6

static const char *categories[CATEGORY_COUNT] = {
    "Featured projects", "NSMB DS", "NSMB 2 (3DS)", "Kaizo & Hard Hacks", "Latest updates", "About & safety"
};

static PrintConsole top, bottom;
static int category = 0;
static int project_index = 0;
static int view = 0; /* 0 categories, 1 project list, 2 project details */

static int visible_project_count(void) {
    if (category == 0) return (int)PROJECT_COUNT;
    if (category == 1 || category == 2 || category == 3) {
        int count = 0;
        for (int i = 0; i < (int)PROJECT_COUNT; i++) {
            if (category == 3 ? projects[i].kaizo : projects[i].system == category - 1) count++;
        }
        return count;
    }
    return 0;
}

static int project_at(int visible_index) {
    if (category == 0) return visible_index;
    int count = 0;
    for (int i = 0; i < (int)PROJECT_COUNT; i++) {
        if (category == 3 ? projects[i].kaizo : projects[i].system == category - 1) {
            if (count == visible_index) return i;
            count++;
        }
    }
    return 0;
}

static void draw_top(void) {
    consoleSelect(&top);
    printf("\x1b[2J\x1b[H");
    printf("\x1b[46;30m                                                  \x1b[0m\n");
    printf("\x1b[46;30m   NSMBHD SHOP                         CATALOG    \x1b[0m\n");
    printf("\x1b[46;30m                                                  \x1b[0m\n\n");
    printf("\x1b[36m  COMMUNITY PROJECTS\x1b[0m\n");
    printf("\x1b[90m  Browse fan-made DS and 3DS projects.\x1b[0m\n\n");

    if (view == 0) {
        for (int i = 0; i < CATEGORY_COUNT; i++) {
            if (i == category)
                printf("\x1b[46;30m  > %-43s\x1b[0m\n\n", categories[i]);
            else
                printf("\x1b[37m    %-43s\x1b[0m\n\n", categories[i]);
        }
        printf("\x1b[90m  Choose a section with UP / DOWN.\x1b[0m\n");
    } else if (view == 1) {
        int count = visible_project_count();
        printf("\x1b[37m  %s  \x1b[90m(%d projects)\x1b[0m\n\n", categories[category], count);
        for (int i = 0; i < count && i < 9; i++) {
            const Project *p = &projects[project_at(i)];
            if (i == project_index)
                printf("\x1b[46;30m  > %-43.43s\x1b[0m\n", p->title);
            else
                printf("\x1b[37m    %-43.43s\x1b[0m\n", p->title);
            printf("\x1b[90m      %-43.43s\x1b[0m\n\n", p->creator);
        }
        printf("\x1b[90m  A: project details   B: sections\x1b[0m\n");
    } else {
        const Project *p = &projects[project_at(project_index)];
        printf("\x1b[36m  PROJECT DETAILS\x1b[0m\n\n");
        printf("\x1b[37m  %s\x1b[0m\n\n", p->title);
        printf("\x1b[90m  Developer: \x1b[37m%s\x1b[0m\n", p->creator);
        printf("\x1b[90m  Version:   \x1b[37m%s\x1b[0m\n", p->version);
        printf("\x1b[90m  Status:    \x1b[37m%s\x1b[0m\n\n", p->status);
        printf("\x1b[90m  Source: NSMBHD community wiki\x1b[0m\n");
        printf("\x1b[36m  %s\x1b[0m\n", p->url);
    }
}

static void draw_bottom(void) {
    consoleSelect(&bottom);
    printf("\x1b[2J\x1b[H");
    printf("\x1b[44;37m                                                  \x1b[0m\n");
    printf("\x1b[44;37m   NSMBHD SHOP  /  QUICK VIEW                     \x1b[0m\n");
    printf("\x1b[44;37m                                                  \x1b[0m\n\n");

    if (view == 0) {
        printf("\x1b[36m  %s\x1b[0m\n\n", categories[category]);
        if (category == 0)
            printf("\x1b[37m  Browse all projects in one list.\x1b[0m\n");
        else if (category == 1)
            printf("\x1b[37m  New Super Mario Bros. DS hacks\n  and community projects.\x1b[0m\n");
        else if (category == 2)
            printf("\x1b[37m  New Super Mario Bros. 2 projects\n  for Nintendo 3DS.\x1b[0m\n");
        else if (category == 3)
            printf("\x1b[37m  Project versions and new releases\n  will be highlighted here.\x1b[0m\n");
        else
            printf("\x1b[37m  Fan-made project directory.\n  Only creator-approved files belong here.\x1b[0m\n");
        printf("\n\x1b[36m  A\x1b[0m  Open section\n");
        printf("\x1b[36m  UP / DOWN\x1b[0m  Navigate\n");
    } else if (view == 1) {
        const Project *p = &projects[project_at(project_index)];
        printf("\x1b[36m  %s\x1b[0m\n\n", p->title);
        printf("\x1b[37m  %s\x1b[0m\n\n", p->description);
        printf("\x1b[90m  Developer: %s\n  Status: %s\x1b[0m\n\n", p->creator, p->status);
        printf("\x1b[36m  A\x1b[0m  View details\n");
        printf("\x1b[36m  B\x1b[0m  Back to sections\n");
    } else {
        const Project *p = &projects[project_at(project_index)];
        printf("\x1b[36m  PROJECT SUMMARY\x1b[0m\n\n");
        printf("\x1b[37m  %s\x1b[0m\n\n", p->description);
        printf("\x1b[90m  This entry is a directory listing,\n  not a hosted game download.\n\n");
        printf("\x1b[33m  DOWNLOADS\x1b[0m\n");
        printf("\x1b[37m  Open the source page shown above\n  to check the creator's current files.\n\n");
        printf("\x1b[36m  B\x1b[0m  Back to project list\n");
    }
    printf("\n\x1b[90m  TOASTLINUS  |  COMMUNITY CATALOG\x1b[0m\n");
}

static void draw_all(void) {
    draw_top();
    draw_bottom();
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
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
                if (category <= 3) { view = 1; project_index = 0; }
                else if (category == 5) view = 2;
                else view = 2;
                changed = true;
            }
            if (down & KEY_B) break;
        } else if (view == 1) {
            int count = visible_project_count();
            if (count > 0 && (down & KEY_UP)) { project_index = (project_index + count - 1) % count; changed = true; }
            if (count > 0 && (down & KEY_DOWN)) { project_index = (project_index + 1) % count; changed = true; }
            if (down & KEY_A && count > 0) { view = 2; changed = true; }
            if (down & KEY_B) { view = 0; changed = true; }
        } else {
            if (down & KEY_B) { view = (category <= 3) ? 1 : 0; changed = true; }
        }

        if (changed) draw_all();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
