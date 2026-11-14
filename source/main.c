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
    {"Newer Super Mario Bros. DS", "Newer Team", "Latest listed", "Finished", "A full New Super Mario Bros. DS community hack with new levels, graphics, enemies and more.", "https://newerteam.com/ds/", 0, "", "", 0},
    {"New Super Mario Bros. Origins", "SKJmin", "1.0a rev2", "Finished", "A community-made New Super Mario Bros. DS hack. Check the project page for current files and notes.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0, "", "", 0},
    {"New Super Luigi Bros. Atlantis Revolution 2.5", "Luigui3060", "2.5", "Finished", "A Luigi-focused DS hack listed in the NSMBHD project index.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0, "", "", 0},
    {"New Super Mario Bros. U DS", "Skylander", "1.10", "Demo", "Project listing. No directly playable CIA, 3DSX or NDS file is verified for the shop yet.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB%29/", 0, "", "", 0},
    {"Kaizo Inspired Mario", "frjd", "Latest listed", "Cancelled (2 levels)", "A Kaizo-inspired New Super Mario Bros. DS project; the community list marks it cancelled after two levels.", "https://jul.rustedlogic.net/thread.php?id=9237", 0, "", "", 0},
    {"New Super Kazio Bros.", "Joe", "Beta", "Cancelled (W1 + 11 levels)", "A difficult New Super Mario Bros. DS project listed as a beta; the community index marks it cancelled.", "https://nsmbhd.net/thread/4177-new-super-kaizo-bros/", 0, "", "", 0},
    {"Newer Super Mario Bros. 2", "Aaron Mallie", "Latest listed", "Finished", "A full New Super Mario Bros. 2 project for Nintendo 3DS.", "https://nsmbhd.net/wiki/List_of_Hacks_%28NSMB2%29/", 1, "", "", 0},
    {"More Super Mario Bros. 3DS", "KarlMaa", "Demo", "Demo", "A New Super Mario Bros. 2 community project for Nintendo 3DS.", "https://nsmbhd.net/thread/5551-more-super-mario-bros-3ds-demo/", 1, "", "", 0},
    {"New Super Mario Bros. 2 Anew!", "GreyMask", "1.2", "Demo", "A New Super Mario Bros. 2 hack with a first-world demo listed by the community wiki.", "https://nsmbhd.net/thread/5182-new-super-mario-bros-2-anew-demo-v-1-2/", 1, "", "", 0},
    {"New Super Mario Bros. 2 Mastermode!!", "NZia", "Latest listed", "Demo", "A challenge-focused NSMB2 project with a demo listing.", "https://nsmbhd.net/thread/5560-nsmb2-mastermode-demo/", 1, "", "", 0},
    {"New Super Mario Bros. 2 Take 2", "ThorgeLP", "Latest listed", "Demo", "A community project with a four-level demo listing.", "https://kuribo64.net/board/thread.php?id=3552", 1, "", "", 0}
};
#define PROJECT_COUNT (sizeof(projects) / sizeof(projects[0]))
#define CATEGORY_COUNT 5

static const char *categories[CATEGORY_COUNT] = {
    "Featured projects", "NSMB DS", "NSMB 2 (3DS)", "Latest updates", "About & safety"
};


static char download_status[128] = "Select a project and press X to download.";

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

static int visible_project_count(void) {
    if (category == 0) return (int)PROJECT_COUNT;
    if (category == 1 || category == 2) {
        int count = 0;
        for (int i = 0; i < (int)PROJECT_COUNT; i++)
            if (projects[i].system == category - 1) count++;
        return count;
    }
    return 0;
}

static int project_at(int visible_index) {
    if (category == 0) return visible_index;
    int count = 0;
    for (int i = 0; i < (int)PROJECT_COUNT; i++) {
        if (projects[i].system == category - 1) {
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
            printf("\x1b[37m  Fan-made project directory.\n  Only approved CIA, 3DSX or NDS files belong here.\x1b[0m\n");
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
        printf("\x1b[33m  PROJECT ACTIONS\x1b[0m\n");
        printf("\x1b[36m  Y  Open official project page\x1b[0m\n");
        if (p->redistribution_authorized && p->download_url[0])
            printf("\x1b[36m  X  Download approved game file\x1b[0m\n");
        else
            printf("\x1b[90m  No playable CIA/3DSX/NDS file yet\x1b[0m\n");
        printf("\x1b[36m  B  Back to project list\x1b[0m\n");
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
                download_project(&projects[project_at(project_index)]);
                changed = true;
            }
            if ((down & KEY_Y) && category <= 2) {
                open_project_page(&projects[project_at(project_index)]);
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
