# 6510
MOS6510 Emulator

![6510 - Commodore 64 Internals, 6510 CPU and 6581 SID. source: https://en.wikipedia.org/wiki/MOS_Technology_6510 ](https://upload.wikimedia.org/wikipedia/commons/2/22/MOS_Technologies_large.jpg)

DEPENDENCIES
------------
Emscripten:  
https://emscripten.org/


Build
-----
cd emcc-build
make

RUN
---
emcc-build
python3 -m http.server

BROWSER
-------
http://127.0.0.1:8000/MOS6510.html
