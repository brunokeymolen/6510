// https://www.jamesfmackenzie.com/2019/12/01/webassembly-graphics-with-sdl/
// https://emscripten.org/docs/getting_started/downloads.html

#include <SDL.h>
#include <emscripten.h>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <atomic>
#include <sys/time.h>

#include "../A64Core/CBM64Main.h"
#include "../A64Core/General.h"

SDL_Surface *screen;
CBM64Main* cbm64;
unsigned char* screenBuffer; 

class HiresTimeImpl : public CHiresTime {
public:
    HiresTimeImpl(){
        start_ = std::chrono::high_resolution_clock::now();
    }
public:
    double GetMicroseconds() {
        auto t2 = std::chrono::high_resolution_clock::now();
        
        auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - start_);
        std::chrono::duration<long, std::micro> int_usec = int_ms;

        return int_usec.count();
    }

	int GetMicrosecondsLo() {
		static timeval t;
		gettimeofday(&t, NULL);	
		return ((t.tv_sec * 1000000) + t.tv_usec);
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};


uint8_t *screenbuffer_[2];
std::atomic<int> *activescreen_;
class EMCScreen : public CVICHWScreen {
public:
    EMCScreen() {
        activescreen_ = new std::atomic<int>();
        *activescreen_ = 0;
        for (int i=0; i<2; i++) {
            screenbuffer_[i] = (uint8_t*)calloc(320*200, sizeof(uint8_t));
        }
    }
    ~EMCScreen() {
        for (int i=0; i<2; i++) {
            free(screenbuffer_[i]);
        }        
        delete(activescreen_);
    }
public:
    void DrawPixels(u8* screenBuffer, VICRect* area) {
        /*  
       int a = *activescreen_ + 1 % 2;
      std::cout << "drawpix area : " << area->width << "," << area->height 
          << "," << area->x << "," << area->y << std::endl; 
       *activescreen_ = a;
       
       for (int x=0; x<area->width; x++) {
           for (int y=0; y<area->height; y++) {
                screenbuffer_[0][ ((y+area->y)*320) + (x+area->x) ] = screenBuffer[y*area->width + x]; 
                //screenbuffer_[0][ (y*320) + (x) ] = screenBuffer[y*area->width + x]; 
           }
       }
*/
       //memcpy(screenbuffer_[0], screenbuffer_, 320*200);
    }
    void DrawChar(u16 address, u8 c) {
      std::cout << "drawchar " << c << std::endl; 

    }
    void DrawChars(u8* memory) {
      std::cout << "drawchars : " << memory << std::endl; 

    }
public:
};



void drawScreen() {
    std::cout << "UI loop" << std::endl;
    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

    Uint8 *pixels = (Uint8*)screen->pixels;
   
    uint8_t darkBLue[4] = {78, 69, 152, 0};
    uint8_t lightBLue[4] = {154, 145, 221, 0};   


    int a = *activescreen_; 
    for (int x=0;x<320;x++) {
        for (int y=0;y<200;y++) {
            //if (x < 30 || y == 5) {
			uint8_t b = screenBuffer[ (x/8) + (y * 40 /*(320/8)*/ )];

			if((b & (1<< (x%8))) > 0){
            
            //if (screenBuffer[y*320+x] > 0) {
                memcpy(pixels+(y*320*4)+(x*4), lightBLue, 4);
            } else {
                memcpy(pixels+(y*320*4)+(x*4), darkBLue, 4); 
            }
        }
    }

    /*  
    for (int i=0; i < 256000; i=i+4) {
        if (screenbuffer_[a][i/4] > 0) {
            memcpy(pixels+i, lightBLue, 4);
        } else {
            memcpy(pixels+i, darkBLue, 4);
        }
    }
    */

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    SDL_Flip(screen);

    std::cout << "Tick from UI loop" << std::endl;
    for (int i=0;i<1500;i++) {
        cbm64->Tick();
    }
}


void drawScreen1() {
    std::cout << "UI loop" << std::endl;
    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

    Uint8 *pixels = (Uint8*)screen->pixels;
   
    uint8_t darkBLue[4] = {78, 69, 152, 0};
    uint8_t lightBLue[4] = {154, 145, 221, 0};   

    int a = *activescreen_; 
    for (int i=0; i < 256000; i=i+4) {
        if (screenbuffer_[a][i/4] > 0) {
            memcpy(pixels+i, lightBLue, 4);
        } else {
            memcpy(pixels+i, darkBLue, 4);
        }
    }

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    SDL_Flip(screen);

    std::cout << "Tick from UI loop" << std::endl;
    for (int i=0;i<500;i++) {
        cbm64->Tick();
    }
}

void drawRandomPixelsORI() {
    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

    //uint_t *screenbuffer_[2];
    Uint8 *pixels = (Uint8*)screen->pixels;
    
    for (int i=0; i < 1048576; i++) {
        char randomByte = rand() % 255;
        pixels[i] = randomByte;
    }

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    SDL_Flip(screen);
}



HiresTimeImpl* hiresTime_ = NULL;
EMCScreen* emcScreen_ = NULL;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(320, 200, 32, SDL_SWSURFACE);
   
    cbm64 = new CBM64Main();
	cbm64->Init();
	
    //Set the hires time callbacks
    hiresTime_ = new HiresTimeImpl();
	cbm64->SetHiresTimeProvider(hiresTime_);

	//Subscribe Host hardware related VIC Code
    emcScreen_ = new EMCScreen();
    CMOS6569* vic = cbm64->GetVic();
	screenBuffer = vic->RegisterHWScreen(emcScreen_);
	//[vicView SetScreenBuffer:screenBuffer];

    
    //cbm64->Run();

    std::cout << "draw some" << std::endl;
    emscripten_set_main_loop(drawScreen, 60, 1);
    std::cout << "end draw some" << std::endl;
}
