

CORE = ../A64Core/MOS6510.cpp \
    ../A64Core/BasicRom.cpp \
    ../A64Core/Bus.cpp \
    ../A64Core/CBM64Main.cpp \
    ../A64Core/CharRom.cpp \
    ../A64Core/Device.cpp \
    ../A64Core/KernalRom.cpp \
    ../A64Core/MOS6510Debug.cpp \
    ../A64Core/MOS6526A.cpp \
    ../A64Core/MOS6569.cpp \
    ../A64Core/Ram.cpp \
    ../A64Core/Util.cpp 

EMCC = ../emcc/main.cpp

SRC = $(CORE) $(EMCC)

all: init compile link 

init:
	cp -R ../roms .

compile: $(SRC)
	emcc -c $(SRC) 

link:
	emcc *.o -o MOS6510.html --preload-file roms

clean:
	rm *.o
	rm *.js
	rm *.html
	rm *.wasm
