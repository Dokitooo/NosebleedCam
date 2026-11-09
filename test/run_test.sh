#!/bin/sh
# usage: run_test.sh STOCK_EXE NITROUS_EXE   (needs gcc and python3)
set -e
cd "$(dirname "$0")"
gcc -O1 -w -o /tmp/nb_test nb_test.c
python3 - "$1" "$2" <<'P'
import subprocess, sys, struct
stock, nitro = sys.argv[1], sys.argv[2]
S = open(stock, 'rb').read(); N = open(nitro, 'rb').read()
def off(va): return va - 0x1002000 + 0xC00400
for kz in (1, 0):
    print(subprocess.run(['/tmp/nb_test', stock, '/tmp/nb_out.exe', str(kz), '58', '100'], capture_output=True, text=True).stdout.strip())
    O = open('/tmp/nb_out.exe', 'rb').read()
    tabs = [(off(0x1028AB8), 0xA8), (off(0x1028BB8), 0xA8)]
    diff_stock = [i for i in range(len(S)) if S[i] != O[i]]
    outside = [i for i in diff_stock if not any(a <= i < a + n for a, n in tabs)]
    # expected: nitrous tables, lens block (+0x74..+0x9B) stock when kz
    bad = 0
    for a, n in tabs:
        for i in range(n):
            want = N[a + i] if not (kz and 0x74 <= i < 0x9C) else (struct.pack('<2f', 58.0, 100.0)[i - 0x74] if i < 0x7C else S[a + i])
            if O[a + i] != want: bad += 1
    zoomA = struct.unpack_from('<4f', O, tabs[0][0] + 0x74)
    print('  KeepStockZoom=%d: bytes outside the two tables changed: %d, table bytes wrong: %d, lens (+74,+78,+7C,+80) now %s' % (kz, len(outside), bad, tuple(round(x, 1) for x in zoomA)))
    assert not outside and not bad
print('OK')
P
