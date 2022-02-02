// https://www.jamesfmackenzie.com/2019/12/01/webassembly-graphics-with-sdl/
// https://emscripten.org/docs/getting_started/downloads.html

#include <chrono>
#include <ctime>
#include <atomic>
#include <sys/time.h>
#include <unistd.h>

#include "A64Core/CBM64Main.h"
#include "A64Core/General.h"

CBM64Main* cbm64 = NULL;

static char CMOS6569TextMap[128] = 
				 {    '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
					  'P','Q','R','S','T','U','V','W','X','Y','Z','[','x',']','^','x',
                      ' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/',
                      '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
                      'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x',
                      'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x',
                      ' ','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x',
                      'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x' };


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
    }
    void DrawChar(u16 address, u8 c) {
    }
    void DrawChars(u8* memory) {
      std::cout << "\033[0;0H" << std::endl; 
      std::cout << "\u001b[44m" << std::endl; 
      for (int y=0;y<25;y++) {
          for (int x=0;x<40;x++) {
            uint8_t m = memory[y*40+x];
            char c = '_';
            if (m < 128) {
                c = CMOS6569TextMap[m]; 
            }
            std::cout << c;
          }
          std::cout << std::endl;
      }
      std::cout << "\u001b[0m" << std::endl; 
    }
public:
};



void draw() {

    bool run = true;

    while (run) {
        cbm64->Tick();
    }

}



HiresTimeImpl* hiresTime_ = NULL;
EMCScreen* emcScreen_ = NULL;

int main(int argc, char* argv[]) {
    cbm64 = new CBM64Main();
	cbm64->Init();
	
    //Set the hires time callbacks
    hiresTime_ = new HiresTimeImpl();
	cbm64->SetHiresTimeProvider(hiresTime_);

	//Subscribe Host hardware related VIC Code
    emcScreen_ = new EMCScreen();
    CMOS6569* vic = cbm64->GetVic();
	unsigned char* screenBuffer = vic->RegisterHWScreen(emcScreen_);
	//[vicView SetScreenBuffer:screenBuffer];

    
   // cbm64->Run();
    
    std::cout << "\033c" << std::endl;
    draw();
}


