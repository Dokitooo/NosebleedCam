/* nb_test.c - runs the real nb_apply_table() (src/nb_core.h) on a copy of the stock exe's data and writes the result:
 * run_test.sh compares it with the Nitrous exe. */
#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;
#include "../src/nb_data.h"
#include "../src/nb_core.h"
static int apply(u8 *a, u8 *b, int kz)
{
    return nb_apply_table(a, nb_stock_A, nb_nitro_A, kz) * 10 + nb_apply_table(b, nb_stock_B, nb_nitro_B, kz);
}
int main(int argc, char **argv)
{
    /* argv: stock.exe out.exe keepzoom ; the two tables sit at file offsets (VA - 0x1002000 + 0xC00400) */
    FILE *f = fopen(argv[1], "rb"); long n; u8 *m; int kz = atoi(argv[3]), r1, r2;
    if (argc > 5) { nb_zoom_far = atoi(argv[4]); nb_zoom_close = atoi(argv[5]); }
    fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET); m = malloc(n); if (fread(m, 1, n, f) != (size_t)n) return 1; fclose(f);
    r1 = apply(m + (0x1028AB8 - 0x1002000 + 0xC00400), m + (0x1028BB8 - 0x1002000 + 0xC00400), kz);
    r2 = apply(m + (0x1028AB8 - 0x1002000 + 0xC00400), m + (0x1028BB8 - 0x1002000 + 0xC00400), kz);   /* second run: already in place */
    printf("first run: %d (0 = both patched)  second run: %d (11 = both already in place)\n", r1, r2);
    f = fopen(argv[2], "wb"); fwrite(m, 1, n, f); fclose(f);
    return 0;
}
