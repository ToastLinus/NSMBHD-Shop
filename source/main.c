#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
    const char *title;
    const char *creator;
    const char *version;
    const char *status;
    const char *description;
    const char *url;
    int system;
    const char *download_url;
    const char *file_ext;
    int redistribution_authorized;
} Project;

static const Project projects[] = {
    {"Newer Super Mario Bros. DS", "Newer Team", "1.19", "Finished", "80 neue Level in 8 Welten. Der offizielle Download liefert einen Patch; du brauchst dein eigenes NSMB-DS-Spiel.", "https://newerteam.com/ds/", 0, "", "", 0},
    {"New Super Mario Bros. Origins", "SKJmin", "1.0a rev2", "Finished", "Vollständiger NSMB-DS-Hack. Download-Link in der offiziellen Community-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"New Super Luigi Bros. Atlantis Revolution 2.5", "Luigui3060", "2.5", "Finished", "Vollständiger Luigi-Hack für NSMB DS. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"New Super Mario Advance + Take 2", "MarioFanatic64", "1.2", "Finished", "Vollständiger DS-Hack mit eigenem Levelinhalt. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"New Super Mario Bros.: Second Quest", "Zarxrax", "v8", "Finished", "Vollständiger NSMB-DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"New Super Mario Bros. Nueva Generacion", "Luigui3060", "Latest", "Finished", "Vollständiger NSMB-DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"New Super Toad DS", "alpha", "Beta 2", "Demo", "Demo mit mehreren Welten. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"Newer Summer Sun DS", "JonteFan05", "Latest", "Demo", "Mehrere spielbare Welten; Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"Newer Super Mario Bros. GT", "Luigui3060", "rev3", "Finished", "Vollständiger NSMB-DS-Hack laut Projektliste. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"Newest Super Mario Bros. DS Deluxe Plus", "Newest Atlantis Team DS", "1.02", "Finished", "Vollständiger DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"Newest Super Mario Bros. DS Deluxe", "Newest Atlantis Team DS", "1.06", "Finished", "Vollständiger DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"Newest Super Mario Bros. DS", "Newest Team", "1.01", "Finished", "Vollständiger DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"New-ish Super Mario Bros. DS", "Skylander", "2020", "Finished", "80-Level-NSMB-DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"Super Mario Endless Earth", "MarioFanatic64", "1.04", "Finished", "Vollständiger NSMB-DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"Super Mario Run DS", "ConorDoesHacks", "2.0", "Finished", "Kleiner NSMB-DS-Hack mit einer Welt. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"Super Mario Bros. - The New Worlds", "Arceus", "3.0", "Finished", "Vollständiger DS-Hack laut Projektliste. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"More Super Mario Bras 2: Choose a Path", "pinet", "1.1d", "Finished", "Vollständiger DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"More Super Mario Bras: Beginner's Edition", "pinet", "1.4c", "Finished", "Vollständiger DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"New Super Mario Bros. 5: Clone Tag Team", "MarioFanatic64", "1.3.1", "Finished", "Vollständiger DS-Hack. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"New Super Mario Bros. 5: Clone Tag Team 2", "MarioFanatic64", "1.0.1", "Finished", "Fortsetzung mit eigenen Leveln. Download-Link in der Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB)/", 0, "", "", 0},
    {"New Super Mario Bros. DS Co-op", "ShaneDoyle", "2.5", "Finished", "Koop-Patch mit allen Leveln. Benötigt eine eigene NSMB-DS-Kopie.", "https://github.com/ShaneDoyle/nsmb-coop/releases/tag/v2.5", 0, "", "", 0},
    {"Mario Classic Adventure", "Supah Malleo", "Latest", "Demo", "NSMB2-Demo für Nintendo 3DS. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"More Super Mario Bros. 3DS", "KarlMaa", "Demo", "Demo", "NSMB2-Projekt für 3DS. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"New Super Mario Bros. 2 Anew!", "GreyMask", "1.2", "Demo", "NSMB2-Demo mit erster Welt. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"New Super Mario Bros. 2 Mastermode!!", "NZia", "Latest", "Demo", "Challenge-Demo für NSMB2. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"New Super Mario Bros. 2 Return to the Pumpkin Boneyard", "Sierra", "1.1", "Finished", "NSMB2-Mod mit einer vollständigen Welt. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"New Super Mario Bros. 2 Take 2", "ThorgeLP", "Latest", "Demo", "NSMB2-Demo mit vier Leveln. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"New Super Mario Bros. 2 U: Gold Edition", "RaptorBR", "2.1", "Demo", "NSMB2-Demo. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"New Super Mario Bros. 2.5: The Adventure Unfolds", "Imnotstaple", "Final Demo", "Demo", "NSMB2-Finaldemo. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"New Super Mario Redux", "TRS", "Latest", "Demo", "NSMB2-Demo. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"Newer Super Mario Bros. 2", "Aaron Mallie", "Latest", "Finished", "Vollständiger NSMB2-Mod laut Projektliste. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"Sunset Shores", "SMBNext Team", "Latest", "Finished", "NSMB2-Mod. Benötigt dein eigenes NSMB2-Spiel.", "https://smbnext.net/sunsetshores", 1, "", "", 0},
    {"Super Mario Bros. 3DX", "Astroclimber26", "Latest", "Demo", "NSMB2-Demo mit mehreren Leveln. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"Super Mario Bros. Next: The Lost Levels", "SMBNext Team", "Parts 1 and 2", "Finished", "Zusätzliche NSMB2-Level. Benötigt dein eigenes NSMB2-Spiel.", "https://smbnext.net/lostlevels", 1, "", "", 0},
    {"Super Mario Bros. Next Halloween Special DX", "SMBNext Team", "v7", "Finished", "NSMB2-Mod mit Halloween-Thema. Benötigt dein eigenes NSMB2-Spiel.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0},
    {"Turbo Super Mario Bros. 2", "Turbø Pingouin", "Latest", "Finished", "NSMB2-Mod mit einer Welt. Download-Link in der NSMB2-Projektliste.", "https://nsmbhd.net/wiki/List_of_Hacks_(NSMB2)/", 1, "", "", 0}
};
#define PROJECT_COUNT (sizeof(projects) / sizeof(projects[0]))
#define CATEGORY_COUNT 5

static const char *categories[CATEGORY_COUNT] = {
    "Featured projects", "NSMB DS", "NSMB 2 (3DS)", "Latest updates", "About & safety"
};


static char download_status[128] = "Projekt wählen und X für die Downloadseite drücken.";

static void open_project_page(const Project *p) {
    if (!p->url || !p->url[0]) {
        snprintf(download_status, sizeof(download_status), "No project page is configured.");
        return;
    }
    size_t length = strlen(p->url);
    if (length >= 0x1000) {
        snprintf(download_status, sizeof(download_status), "Project URL is too long.");
        return;
    }
    char browser_buffer[0x1000] = {0};
    memcpy(browser_buffer, p->url, length);
    browser_buffer[length] = 0;
    aptLaunchSystemApplet(APPID_WEB, browser_buffer, length + 1, 0);
    snprintf(download_status, sizeof(download_status), "Returned from project page.");
}

static bool download_project(const Project *p) {
    /* Only directly playable/homebrew package formats are allowed in this shop. */
    bool allowed_type = strcmp(p->file_ext, "cia") == 0 ||
                        strcmp(p->file_ext, "3dsx") == 0 ||
                        strcmp(p->file_ext, "nds") == 0;
    if (!p->redistribution_authorized || !p->download_url[0] || !allowed_type) {
        snprintf(download_status, sizeof(download_status), "No creator-approved file is configured yet.");
        return false;
    }

    mkdir("sdmc:/3ds", 0777);
    mkdir("sdmc:/3ds/NSMBHD-Shop", 0777);
    char path[160];
    snprintf(path, sizeof(path), "sdmc:/3ds/NSMBHD-Shop/%s.%s",
             p->title[0] ? p->title : "project", p->file_ext);

    httpcContext context;
    Result ret = httpcOpenContext(&context, HTTPC_METHOD_GET, p->download_url, 1);
    if (R_FAILED(ret)) {
        snprintf(download_status, sizeof(download_status), "Could not connect to download server.");
        return false;
    }

    if (strncmp(p->download_url, "https://", 8) == 0)
        httpcAddDefaultCert(&context, SSLC_DefaultRootCert_DigiCert_EV);

    ret = httpcBeginRequest(&context);
    u32 status = 0;
    if (R_SUCCEEDED(ret)) ret = httpcGetResponseStatusCode(&context, &status);
    if (R_FAILED(ret) || status < 200 || status >= 300) {
        httpcCloseContext(&context);
        snprintf(download_status, sizeof(download_status), "Download server returned an error.");
        return false;
    }

    FILE *out = fopen(path, "wb");
    if (!out) {
        httpcCloseContext(&context);
        snprintf(download_status, sizeof(download_status), "Could not write to the SD card.");
        return false;
    }

    u8 buffer[0x4000];
    bool ok = true;
    while (aptMainLoop()) {
        u32 received = 0;
        ret = httpcDownloadData(&context, buffer, sizeof(buffer), &received);
        if (received && fwrite(buffer, 1, received, out) != received) {
            ok = false;
            break;
        }
        if (ret == HTTPC_RESULTCODE_DOWNLOADPENDING) continue;
        if (R_FAILED(ret) || received == 0) {
            if (R_FAILED(ret)) ok = false;
            break;
        }
    }

    fclose(out);
    httpcCloseContext(&context);
    if (!ok) {
        remove(path);
        snprintf(download_status, sizeof(download_status), "Download failed; incomplete file removed.");
        return false;
    }
    snprintf(download_status, sizeof(download_status), "Saved to /3ds/NSMBHD-Shop on SD card.");
    return true;
}

static PrintConsole top, bottom;
static int category = 0;
static int project_index = 0;
static int view = 0; /* 0 categories, 1 project list, 2 project details */

static bool has_download_page(const Project *p) {
    return p->url && p->url[0] != '\0';
}

static int visible_project_count(void) {
    int count = 0;
    for (int i = 0; i < (int)PROJECT_COUNT; i++) {
        if (has_download_page(&projects[i]) &&
            (category == 0 || projects[i].system == category - 1))
            count++;
    }
    return count;
}

static int project_at(int visible_index) {
    int count = 0;
    for (int i = 0; i < (int)PROJECT_COUNT; i++) {
        if (has_download_page(&projects[i]) &&
            (category == 0 || projects[i].system == category - 1)) {
            if (count == visible_index) return i;
            count++;
        }
    }
    return -1;
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
        printf("\x1b[37m  %s  \x1b[90m(%d Mario-Projekte)\x1b[0m\n\n", categories[category], count);
        if (count == 0)
            printf("\x1b[33m  Keine Projekte mit Downloadseite gefunden.\x1b[0m\n\n");
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
        int selected = project_at(project_index);
        if (selected < 0) {
            printf("\x1b[33m  No project download pages available.\x1b[0m\n");
            return;
        }
        const Project *p = &projects[selected];
        printf("\x1b[36m  PROJECT DETAILS\x1b[0m\n\n");
        printf("\x1b[37m  %s\x1b[0m\n\n", p->title);
        printf("\x1b[90m  Developer: \x1b[37m%s\x1b[0m\n", p->creator);
        printf("\x1b[90m  Version:   \x1b[37m%s\x1b[0m\n", p->version);
        printf("\x1b[90m  Status:    \x1b[37m%s\x1b[0m\n\n", p->status);
        printf("\x1b[90m  Projekt- und Downloadseite:\x1b[0m\n");
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
            printf("\x1b[37m  Mario-Projekte mit Downloadseiten.\x1b[0m\n");
        else if (category == 1)
            printf("\x1b[37m  2D-Mario-Projekte für Nintendo DS.\x1b[0m\n");
        else if (category == 2)
            printf("\x1b[37m  NSMB2-Projekte für Nintendo 3DS.\x1b[0m\n");
        else if (category == 3)
            printf("\x1b[37m  Neuere Mario-Projekte.\x1b[0m\n");
        else
            printf("\x1b[37m  Downloadseiten der Projekt-Ersteller.\x1b[0m\n");
        printf("\n\x1b[36m  A\x1b[0m  Open section\n");
        printf("\x1b[36m  UP / DOWN\x1b[0m  Navigate\n");
    } else if (view == 1) {
        int selected = project_at(project_index);
        if (selected < 0) {
            printf("\x1b[33m  Keine Downloadseiten gefunden.\x1b[0m\n");
            printf("\x1b[90m  Einträge erscheinen erst mit einer\n  freigegebenen CIA-, 3DSX- oder NDS-Datei.\x1b[0m\n");
        } else {
            const Project *p = &projects[selected];
            printf("\x1b[36m  %s\x1b[0m\n\n", p->title);
            printf("\x1b[37m  %s\x1b[0m\n\n", p->description);
            printf("\x1b[90m  Developer: %s\n  Status: %s\x1b[0m\n\n", p->creator, p->status);
            printf("\x1b[36m  A\x1b[0m  View details\n");
        }
        printf("\x1b[36m  B\x1b[0m  Back to sections\n");
    } else {
        int selected = project_at(project_index);
        if (selected < 0) {
            printf("\x1b[33m  Keine Projektseiten vorhanden.\x1b[0m\n");
            printf("\x1b[90m  X öffnet die Projekt- und Downloadseite im Browser.\x1b[0m\n");
            printf("\n\x1b[36m  B\x1b[0m  Zurück\n");
        } else {
            const Project *p = &projects[selected];
            printf("\x1b[36m  PROJECT SUMMARY\x1b[0m\n\n");
            printf("\x1b[37m  %s\x1b[0m\n\n", p->description);
            printf("\x1b[33m  DOWNLOAD / PROJEKTSEITE\x1b[0m\n");
            printf("\x1b[36m  X  Downloadseite im Browser öffnen\x1b[0m\n");
            printf("\x1b[90m  Viele Fan-Projekte sind Patches und brauchen das Originalspiel.\x1b[0m\n");
            printf("\x1b[36m  B  Back to project list\x1b[0m\n");
        }
    }
    printf("\n\x1b[90m  %s\x1b[0m\n", download_status);
    printf("\x1b[90m  TOASTLINUS  |  COMMUNITY CATALOG\x1b[0m\n");
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
    httpcInit(0);
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
                if (category <= 2) { view = 1; project_index = 0; }
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
            if ((down & KEY_X) && category <= 2) {
                int selected = project_at(project_index);
                if (selected >= 0) open_project_page(&projects[selected]);
                changed = true;
            }

            if (down & KEY_B) { view = (category <= 2) ? 1 : 0; changed = true; }
        }

        if (changed) draw_all();
        gspWaitForVBlank();
    }

    httpcExit();
    gfxExit();
    return 0;
}
