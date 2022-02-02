// https://www.jamesfmackenzie.com/2019/12/01/webassembly-graphics-with-sdl/
// https://emscripten.org/docs/getting_started/downloads.html

#include <chrono>
#include <ctime>
#include <atomic>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <termios.h>

#include "A64Core/CBM64Main.h"
#include "A64Core/General.h"

CBM64Main* cbm64 = NULL;
bool _run = true;

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

//https://stackoverflow.com/questions/36428098/c-how-to-check-if-my-input-bufferstdin-is-empty
static int getch_lower_(int block)
{
    struct termios tc = {};
    int status;
    char rdbuf;
    // retrieve initial settings.
    if (tcgetattr(STDIN_FILENO, &tc) < 0)
        perror("tcgetattr()");
    // non-canonical mode; no echo.
    tc.c_lflag &= ~(ICANON | ECHO);
    tc.c_cc[VMIN] = block ? 1 : 0; // bytes until read unblocks.
    tc.c_cc[VTIME] = 0; // timeout.
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tc) < 0)
        perror("tcsetattr()");
    // read char.
    if ((status = read(STDIN_FILENO, &rdbuf, 1)) < 0)
        perror("read()");
    // restore initial settings.
    tc.c_lflag |= (ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSADRAIN, &tc) < 0)
        perror("tcsetattr()");
    return (status > 0) ? rdbuf : EOF;
}

std::vector<char> getKeystroke() {
    std::vector<char> keystroke;
    int cc = getch_lower_(1);
    keystroke.push_back(cc);
    while (cc != -1) {
        cc = getch_lower_(0);
        if (cc != -1) {
            keystroke.push_back(cc);
        }
    }
    return keystroke;
}



//https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

const char ESC[2] = { 27, -1 };
const char KEYUP[4] = { 27, 91, 65, -1 }; 
const char KEYDOWN[4] = { 27, 91, 66, -1 }; 
const char KEYLEFT[4] = { 27, 91, 68, -1 }; 
const char KEYRIGHT[4] = { 27, 91, 67, -1 }; 

HiresTimeImpl* hiresTime_ = NULL;
EMCScreen* emcScreen_ = NULL;

bool isKeystroke(std::vector<char>& keystroke, const char* k) {
    int len = 0;
    while (k[len] != -1) {len++;}
    if (len != keystroke.size()) {
        return false;
    }
    for (int x=0; x<keystroke.size(); x++) {
        if (keystroke[x] != k[x]) {
            return false;
        }
    }
    return true;
}

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


    std::thread t([=]{
        while(_run) {
            std::vector<char> keystroke = getKeystroke();
            //https://sta.c64.org/cbm64pet.html
            char c = keystroke[0];
            if (c >= 'a' && c <= 'z') {
                c += 'A' - 'a';
            } else if (c == 10) {
                c = 13;
            } else if (isKeystroke(keystroke, ESC)) {
                c = 3;
            } else if (isKeystroke(keystroke, KEYUP)) {
                c = 145;
            } else if (isKeystroke(keystroke, KEYDOWN)) {
                c = 17;
            } else if (isKeystroke(keystroke, KEYLEFT)) {
                c = 157;
            } else if (isKeystroke(keystroke, KEYRIGHT)) {
                c = 29;
            } else if (c == 127) {
                c = 20;
            }
		    cbm64->GetCia1()->AddKeyStroke(c);
        }
    });

    std::cout << "\033c" << std::endl;
    draw();
}


