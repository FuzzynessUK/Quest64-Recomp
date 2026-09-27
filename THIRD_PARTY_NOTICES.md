# Third-party notices

Quest 64: Recompiled includes, or is built from, the work below. Each is used
under its own licence; the notices those licences require are reproduced
here. Full licence texts ship with each component (paths given), and the
release archive carries this file.

Quest 64 itself is not included. The game data comes from the player's own
ROM.

---

## Merrow (Quest 64 randomizer)

- Source: https://github.com/hangedmandesign/merrow
- Used for: the Randomizer tab. Its option logic, tables and tooltips are
  ported to C++ in `src/game/randomizer/` (`merrow_data.cpp` and
  `merrow_mapdata.cpp` are generated from Merrow's data by
  `tools/convert_merrow_*.pl`).
- Licence: MIT

```
MIT License

Merrow is copyright (c) 2021 Jonah Davidson (Hangedman)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## GamepadMotionHelpers

- Source: https://github.com/JibbSmart/GamepadMotionHelpers
- Used for: gyro aiming. Controller motion-sensor calibration and sensor
  fusion in `src/game/input.cpp`, from `lib/GamepadMotionHelpers`.
- Licence: MIT

```
MIT License

Copyright (c) 2020-2023 Julian "Jibb" Smart

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## Code

| Component | Source | Licence | Copyright | Licence text |
|---|---|---|---|---|
| N64ModernRuntime (librecomp, ultramodern) | github.com/N64Recomp/N64ModernRuntime | GPL-3.0 | N64ModernRuntime contributors | `lib/N64ModernRuntime/COPYING` |
| N64Recomp | github.com/N64Recomp/N64Recomp | MIT | (c) 2024 Wiseguy | `lib/N64ModernRuntime/N64Recomp/LICENSE` |
| Zelda64Recomp (project template) | github.com/Zelda64Recomp/Zelda64Recomp | GPL-3.0 | Zelda64Recomp contributors | — |
| RT64 | github.com/rt64/rt64 | MIT | (c) 2024 RT64 Contributors | `lib/rt64/LICENSE` |
| RmlUi | github.com/mikke89/RmlUi | MIT | (c) 2008-2014 CodePoint Ltd, Shift Technology Ltd, and contributors | `lib/RmlUi/LICENSE.txt` |
| lunasvg | github.com/sammycage/lunasvg | MIT | (c) 2020 Nwutobo Samuel Ugochukwu | `lib/lunasvg/LICENSE` |
| sse2neon | github.com/DLTcollab/sse2neon | MIT | (c) 2015-2024 SSE2NEON Contributors | `lib/sse2neon/LICENSE` |
| SlotMap | lib/SlotMap | MIT | SlotMap authors | `lib/SlotMap/README.md` |
| nlohmann/json | github.com/nlohmann/json | MIT | (c) 2013-2025 Niels Lohmann | `lib/N64ModernRuntime/thirdparty/json/json.hpp` |
| miniz | github.com/richgel999/miniz | MIT | Rich Geldreich and contributors | `lib/N64ModernRuntime/thirdparty/miniz/LICENSE` |
| APCpp | github.com/N00byKing/APCpp | LGPL-2.1 (dynamically linked as `APCpp.dll`) | APCpp contributors | `lib/APCpp/LICENSE`, shipped as `APCpp-LICENSE.txt` |
| IXWebSocket (via APCpp) | github.com/machinezone/IXWebSocket | BSD-3-Clause | (c) 2018 Machine Zone, Inc. | `lib/APCpp/IXWebSocket/LICENSE.txt` |
| JsonCpp (via APCpp) | github.com/open-source-parsers/jsoncpp | MIT / public domain | (c) 2007-2010 Baptiste Lepilleur and The JsonCpp Authors | `lib/APCpp/jsoncpp/LICENSE` |
| zlib (via APCpp) | github.com/madler/zlib | zlib | (C) 1995-2024 Jean-loup Gailly and Mark Adler | `lib/APCpp/zlib/LICENSE` |
| Mbed TLS (via APCpp) | github.com/Mbed-TLS/mbedtls | Apache-2.0 | The Mbed TLS Contributors | inside `lib/APCpp/mbedtls-3.6.4.tar.bz2` |
| SDL2 | libsdl.org | zlib | (C) 1997-2024 Sam Lantinga | SDL2 release archive |

## Fonts

| Font | Licence | Copyright | Licence text |
|---|---|---|---|
| Jost | SIL Open Font License 1.1 | (c) 2020 The Jost Project Authors (github.com/indestructible-type) | `assets/Jost-OFL.txt` |
| PromptFont | SIL Open Font License 1.1 | Copyright 2018-2023 Yukari "Shinmera" Hafner; based on Xolonium, Copyright 2011-2016 Severin Meyer. PromptFont by Yukari "Shinmera" Hafner, available at https://shinmera.com/promptfont. Console button glyphs are trademarks of their respective owners. | `assets/promptfont/LICENSE.txt` |
| Lato (LatoLatin) | SIL Open Font License 1.1 | (c) 2010-2015 Łukasz Dziedzic | scripts.sil.org/OFL |
| Noto Emoji | SIL Open Font License 1.1 | (c) Google LLC | scripts.sil.org/OFL |
