#!/bin/bash

[ -d roms ] || mkdir roms
curl http://www.zimmers.net/anonftp/pub/cbm/firmware/computers/c64/kernal.901227-02.bin -o roms/KERNAL.ROM
curl http://www.zimmers.net/anonftp/pub/cbm/firmware/computers/c64/characters.901225-01.bin -o roms/CHAR.ROM
curl http://www.zimmers.net/anonftp/pub/cbm/firmware/computers/c64/basic.901226-01.bin -o roms/BASIC.ROM

