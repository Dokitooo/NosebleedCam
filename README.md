# NosebleedCam 1.0 — the Nitrous nosebleed camera for NBA 2K14 (PC)

NosebleedCam gives the stock `nba2k14.exe` the *Nosebleeds* camera of the Nitrous exe, without replacing your exe. Pick *Camera = Nosebleeds* in the game's camera settings as usual. It is a [SLAM](https://github.com/Dokitooo/SLAM) mod.

**Download:** [NosebleedCam.zip](https://github.com/Dokitooo/NosebleedCam/releases/latest/download/NosebleedCam.zip) (latest release) ·
[all releases](https://github.com/Dokitooo/NosebleedCam/releases)

## Requirements

- NBA 2K14 PC
- [SLAM](https://github.com/Dokitooo/SLAM/releases/latest/download/SLAM.zip) 1.0.0 or newer

## Install

1. Install SLAM first (see its README).
2. Extract the zip into the game folder (the one with `nba2k14.exe`). It contains only:
   ```
   SLAM\NosebleedCam\NosebleedCam.asi
   SLAM\NosebleedCam\NosebleedCam.ini
   ```
3. Start the game. The mod list at the top left shows `NosebleedCam 1.0`.

The Nosebleeds preset lives in two parameter tables inside `nba2k14.exe` (`0x01028AB8`, `0x01028BB8`); for the camera, the Nitrous exe differs from the stock one only there. NosebleedCam writes Nitrous' tables into the running game's memory, only when they still hold the exact stock values, and a watchdog re-applies them if the game resets them. Nothing on disk is changed.

The log is `SLAM\NosebleedCam\NosebleedCam.log`. To remove the mod, delete `SLAM\NosebleedCam`.

## Settings (`NosebleedCam.ini`)

| Setting | What it does |
| --- | --- |
| `Enabled` | `0` = off (the game's own Nosebleeds camera). |
| `KeepStockZoom` | `1` = the stock lens, with the Zoom setting's range set by ZoomFar / ZoomClose. `0` = all of Nitrous' lens values. |
| `ZoomFar / ZoomClose` | The lens at the lowest Zoom and at Zoom 10. Bigger = closer. Default 58 / 100 (stock: 45 / 60). |

## Repository layout

| Path | Contents |
| --- | --- |
| `src/` | The mod (`nosebleedcam.c`, `nb_core.h`, generated `nb_data.h`) |
| `slam/` | SLAM SDK header and glue |
| `build/` | Build scripts (MSVC, clang-cl on Linux) |
| `tools/` | `make_data.py`: writes `nb_data.h` from the stock and Nitrous exes |
| `test/` | `run_test.sh STOCK_EXE NITROUS_EXE`: checks the patch against both exes |
| `package/` | The README and default `.ini` |

## Build

From an *x86 Native Tools Command Prompt for VS*: `build\build_msvc.bat`, or `build/build_linux_clang.sh`. Output: `bin\NosebleedCam.asi`.
On Linux with clang-cl + lld-link (LLVM 18). `slam/slam_glue.c` needs Windows headers: set `MGW` to the
MinGW-w64 `mingw-w64-headers` folder and `GEN` to a folder with a generated `_mingw.h` (see SLAM's `source/build`).

## Verify your download

Every release lists the SHA-256 of the zip in `SHA256SUMS.txt` (also attached to the release).
Compare it with the hash of the file you downloaded:

```powershell
Get-FileHash .\NosebleedCam.zip -Algorithm SHA256
```

```bash
sha256sum -c SHA256SUMS.txt
```

## About antivirus warnings

The ASI file in this mod hooks into `nba2k14.exe` at runtime, which some antivirus heuristics
flag on unknown files. To make the release easy to check:

- The full source is in this repository; the release binary is built from it.
- The binary is not packed, compressed or obfuscated, and the zip has no password.
- SHA-256 checksums are published with each release.

If your antivirus still flags a file, you can submit it as a false positive to the vendor, or
build it yourself from the source.
