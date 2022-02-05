# 6510
MOS6510 Emulator

![6510 - Commodore 64 Internals, 6510 CPU and 6581 SID. source: https://en.wikipedia.org/wiki/MOS_Technology_6510 ](https://upload.wikimedia.org/wikipedia/commons/2/22/MOS_Technologies_large.jpg)

##DEPENDENCIES

Emscripten:  
https://emscripten.org/


##Emscripten (WebAssembly)

###Build

cd emcc-build
make

###RUN

emcc-build
python3 -m http.server

###BROWSER

http://127.0.0.1:8000/MOS6510.html


##Bash

###Build

cmake -S . -B build
cd build
make

###RUN

./mos6510


## Sources
http://users.telenet.be/kim1-6502/6502/proman.html
http://archive.6502.org/datasheets/synertek_programming_manual.pdf
http://6502.org/tutorials/interrupts.html


