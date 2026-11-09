NosebleedCam 1.0 - NBA 2K14 (PC, RLD nba2k14.exe)
=================================================
The "Nosebleeds" camera of the Nitrous nba2k14.exe without replacing your nba2k14.exe.
Pick Camera = Nosebleeds in the game's camera settings as usual.

NEEDS: SLAM 1.0 (the SLAM folder + dinput8.dll next to nba2k14.exe).

INSTALL
  Copy SLAM\NosebleedCam\ into the game's SLAM folder:
      SLAM\NosebleedCam\NosebleedCam.asi
      SLAM\NosebleedCam\NosebleedCam.ini
  The mod list at the top left shows "NosebleedCam 1.0".
  Log: SLAM\NosebleedCam\NosebleedCam.log ("table A ... patched", "table B ... patched").

SETTINGS (NosebleedCam.ini)
  ZoomFar / ZoomClose   the lens at the lowest Zoom and at Zoom 10 (bigger = closer; default 58 / 100)
  KeepStockZoom=1       keep the stock close-up lens and lens curves (0 = all of Nitrous' values)

WHAT IT CHANGES
- The game keeps every camera preset as data inside nba2k14.exe. The Nosebleeds preset has two
  parameter tables (0x01028AB8 and 0x01028BB8); for the camera, the Nitrous exe differs from the stock
  exe only there. NosebleedCam writes Nitrous' tables into the running game's memory. Nothing on disk
  is changed; remove the mod and the game is stock again.
- Zoom: the camera menu's Zoom setting picks the lens between ZoomFar (lowest Zoom) and ZoomClose
  (Zoom 10). Everything else (distance, height, framing, limits) is Nitrous'.
- Safety: the tables are only written when they still hold the exact stock values, and a watchdog
  re-applies them if the game resets them.
