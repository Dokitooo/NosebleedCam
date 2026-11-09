/* nb_core.h - the part of NosebleedCam that decides which bytes go where (no Windows calls, so the native test can run it).
 * Needs u8 and nb_data.h from the including file. */
#define NB_ZOOM_OFF 0x74            /* lens block: what the camera menu's Zoom setting controls */
#define NB_ZOOM_LEN 0x28            /* +0x74..+0x9B: lens range (2 pairs) + 2 lens offset curves */

static int nb_zoom_far = 58, nb_zoom_close = 100;   /* NosebleedCam.ini ZoomFar / ZoomClose */

static int same(const u8 *a, const u8 *b, int n) { int i; for (i = 0; i < n; i++) if (a[i] != b[i]) return 0; return 1; }

/* the table that goes into the game: Nitrous', with the zoom range of the stock exe when keepzoom */
static void nb_wanted(u8 *out, const u8 *nitro, const u8 *stock, int keepzoom)
{
    int i;
    for (i = 0; i < NB_LEN; i++) out[i] = nitro[i];
    if (keepzoom) {
        for (i = NB_ZOOM_OFF; i < NB_ZOOM_OFF + NB_ZOOM_LEN; i++) out[i] = stock[i];
        /* 1.1: the lens at the Zoom setting's two ends (+74 = farthest setting, +78 = closest, 10).
         * Bigger = closer. Stock 45 / 60; 0 = keep the stock value. */
        if (nb_zoom_far > 0)   { float f = (float)nb_zoom_far;   u8 *b = (u8*)&f; for (i = 0; i < 4; i++) out[0x74 + i] = b[i]; }
        if (nb_zoom_close > 0) { float f = (float)nb_zoom_close; u8 *b = (u8*)&f; for (i = 0; i < 4; i++) out[0x78 + i] = b[i]; }
    }
}

/* 0 = patched, 1 = already the wanted table, 2 = not the stock exe (left alone) */
static int nb_apply_table(u8 *tab, const u8 *stock, const u8 *nitro, int keepzoom)
{
    u8 want[NB_LEN];
    nb_wanted(want, nitro, stock, keepzoom);
    if (same(tab, want, NB_LEN)) return 1;
    if (!same(tab, stock, NB_LEN)) return 2;
    { int i; for (i = 0; i < NB_LEN; i++) tab[i] = want[i]; }
    return 0;
}

