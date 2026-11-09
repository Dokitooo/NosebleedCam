/*
 * NosebleedCam 1.0 - NBA 2K14 (PC, RLD nba2k14.exe): the Nitrous nosebleed camera as a script mod, so nobody has to
 * replace nba2k14.exe.
 *
 * What it is: the game's camera presets are plain data in nba2k14.exe's .data section (one 0x30-byte record per camera
 * at 0x0102A488; the code reads them at 0x0042B7C0). The "Nosebleeds" record (name hash 7E35D4F7) points at two
 * parameter tables, 0x01028AB8 and 0x01028BB8 (0xA8 bytes each; the game picks one of the two). The Nitrous exe
 * differs from the stock exe in exactly those tables for the camera (its other changes are not camera related and are
 * NOT copied). This mod writes the Nitrous tables into the running game's memory - nothing on disk is touched.
 *
 * Zoom: the camera update (0x0091FD50) turns the camera menu's Zoom setting into the lens (field of view, camera +0x100)
 * from the table's lens block: +0x74/+0x78 = lens at Zoom min / max, +0x7C/+0x80 = the same for a close-up case,
 * +0x84 and +0x90 = two small lens curves added on top. Stock: 45 / 60 / 95 / 105; Nitrous: 45 / 153 / 240 / 110.
 * With KeepStockZoom=1 (default) that block (+0x74..+0x9B) stays as the stock exe has it, so the zoom is the game's
 * default; every other value (distance, height, framing, limits) is Nitrous'.
 *
 * Safety: the tables are only changed if they still hold the exact stock values (so it never writes into another exe
 * build, and never twice); a watchdog re-applies them if the game ever resets them.
 *
 * Load: SLAM mod (needs SLAM 1.0+): SLAM\NosebleedCam\NosebleedCam.asi + NosebleedCam.ini.
 * Log: SLAM\NosebleedCam\NosebleedCam.log. The two tables are written with SLAM's api->patch, so another
 * mod changing the same camera data is reported in SLAM.log instead of silently fighting this one.
 * Build: build/build_linux_clang.sh (clang-cl + lld-link) - no Windows SDK needed. slam_glue.c holds DllMain
 * and SlamModStart (compiled with <windows.h>); this file starts in mod_start().
 */
#include "win_min.h"
typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32;
#include "nb_data.h"
#define VERSION "1.0"
#include "slam_mod.h"
int _fltused = 0;

static char g_ini[MAX_PATH];
static void s_cpy(char *d, const char *s, int n) { int i = 0; while (s[i] && i < n - 1) { d[i] = s[i]; i++; } d[i] = 0; }
/* NosebleedCam.log in the mod's SLAM folder (SLAM adds the time) */
static void logs(const char *a, const char *b, const char *c)
{
    g_slam->log(g_slamMe, "%s%s%s", a ? a : "", b ? b : "", c ? c : "");
}

static int g_enabled = 1, g_keepzoom = 1;

#include "nb_core.h"

#ifndef NB_TEST
static int readable(const void *p, u32 n)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(p, &mbi, sizeof mbi)) return 0;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return 0;
    return (u32)p + n <= (u32)mbi.BaseAddress + mbi.RegionSize;
}

/* write the wanted table with SLAM's api->patch (registered, conflict-checked) */
static int nb_apply_one(u32 va, const u8 *stock, const u8 *nitro, int keepzoom)
{
    u8 want[NB_LEN]; int r;
    u8 *p = (u8 *)va;
    if (!readable(p, NB_LEN)) return 3;
    nb_wanted(want, nitro, stock, keepzoom);
    if (same(p, want, NB_LEN)) return 1;
    if (!same(p, stock, NB_LEN)) return 2;
    r = g_slam->patch(g_slamMe, p, want, NB_LEN);
    return r == SLAM_OK ? 0 : r == SLAM_ERR_CONFLICT ? 5 : 4;
}

static const char *what(int r) { return r == 0 ? "patched" : r == 1 ? "already in place" : r == 2 ? "not the stock values (other exe build?) - left alone" :
                                        r == 5 ? "another mod already changed it (see SLAM\\SLAM.log) - left alone" : "memory not accessible"; }

static DWORD WINAPI init_thread(LPVOID unused)
{
    DWORD t0 = GetTickCount(); int ra, rb, done = 0;
    (void)unused;
    for (;;) {
        ra = nb_apply_one(NB_VA_A, nb_stock_A, nb_nitro_A, g_keepzoom);
        rb = nb_apply_one(NB_VA_B, nb_stock_B, nb_nitro_B, g_keepzoom);
        if (ra < 2 && rb < 2) { done = 1; break; }
        if (ra == 5 || rb == 5) break;                       /* another mod owns these bytes: don't keep trying */
        /* 2 (other values) can also mean "the game has not filled its data yet": wait, then give up */
        if (GetTickCount() - t0 > 60000) break;
        Sleep(250);
    }
    logs("nosebleed camera table A (0x01028AB8): ", what(ra), 0);
    logs("nosebleed camera table B (0x01028BB8): ", what(rb), 0);
    if (!done) { logs("not applied - this needs the RLD nba2k14.exe (the stock one, TimeDateStamp 52202C7F)", 0, 0); return 0; }
    logs("installed - nosebleed camera of the Nitrous exe", g_keepzoom ? ", stock lens, Zoom setting from ZoomFar to ZoomClose (KeepStockZoom=1)" : ", with the Nitrous zoom range (KeepStockZoom=0)", 0);
    /* watchdog: re-apply if the game resets the tables (cheap: 2 x 168 bytes every 2 s) */
    for (;;) {
        Sleep(2000);
        if (nb_apply_one(NB_VA_A, nb_stock_A, nb_nitro_A, g_keepzoom) == 0 || nb_apply_one(NB_VA_B, nb_stock_B, nb_nitro_B, g_keepzoom) == 0)
            logs("the game reset the camera tables - applied again", 0, 0);
    }
}

/* SLAM calls this once all mods are loaded (slam_glue.c -> SlamModStart) */
int mod_start(void)
{
    const char *ini = g_slam->mod_ini(g_slamMe);
    s_cpy(g_ini, ini, MAX_PATH);
    g_enabled = (int)GetPrivateProfileIntA("NosebleedCam", "Enabled", 1, g_ini);
    g_keepzoom = (int)GetPrivateProfileIntA("NosebleedCam", "KeepStockZoom", 1, g_ini);
    nb_zoom_far = (int)GetPrivateProfileIntA("NosebleedCam", "ZoomFar", 58, g_ini);
    nb_zoom_close = (int)GetPrivateProfileIntA("NosebleedCam", "ZoomClose", 100, g_ini);
    if (nb_zoom_far > 300) nb_zoom_far = 300;
    if (nb_zoom_close > 300) nb_zoom_close = 300;
    logs("NosebleedCam " VERSION " loaded (SLAM ", g_slam->slamVersion, g_enabled ? ")" : ") - Enabled=0 in NosebleedCam.ini: off");
    if (g_enabled) CreateThread(NULL, 0, init_thread, NULL, 0, NULL);
    return 0;
}
#endif
