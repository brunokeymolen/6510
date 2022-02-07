#include <chrono>
#include <ctime>
#include <atomic>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "A64Core/CBM64Main.h"
#include "A64Core/General.h"


class HiresTimeImpl;

CBM64Main* cbm64 = NULL;
bool _run = true;
HiresTimeImpl* hiresTime_ = NULL;
uint64_t total_cycles = 0;
uint64_t start = 0;
char charbuffer[320*200];


static char CMOS6569TextMap[128] = 
{    '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
    'P','Q','R','S','T','U','V','W','X','Y','Z','[','x',']','^','x',
    ' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/',
    '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
    'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x',
    'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x',
    ' ','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x',
    'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x' };

uint64_t now() {
    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return now;
}


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

void Inthandler(int sig)
{
    signal(sig, SIG_IGN);
    _run = false;
}

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
        int cnt_ = 0;
        void DrawChars(u8* memory) {
            memcpy(charbuffer, memory, 320*200);
        }
    public:
};


void runloop() {
    int target_cycles = 1023000; //NTSC
    int cycles_100_ms = target_cycles / 10;
    int cycles = 0;
    start = now();

    while (_run) {
        cycles = 0;
        uint64_t ts = now();
        while(true) {
            cycles += cbm64->Cycle();
            if (cycles >= cycles_100_ms) {
                total_cycles += cycles;
                uint64_t elapsed = now() - ts;
                if (elapsed < 100) {
                    usleep((100-elapsed) * 1000);
                }
                cycles = cycles - cycles_100_ms;
                break;
            }
        }
    }

}

void uiloop() {
    std::cout << "\033[0;0H" << std::endl; 
    std::cout << "\u001b[44m" << std::endl; 
    for (int y=0;y<25;y++) {
        for (int x=0;x<40;x++) {
            uint8_t m = charbuffer[y*40+x];
            char c = '_';
            if (m < 128) {
                c = CMOS6569TextMap[m]; 
            }
            std::cout << c;
        }
        std::cout << std::endl;
    }
    std::cout << "\u001b[0m" << std::endl;

    int64_t elapsed = now()-start;
    if (elapsed > 0) {
        std::cout << "cycles " << 1000*(total_cycles/(now()-start)) << "/sec" << std::endl;
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



const std::vector<char> ESC = { 27 };
const std::vector<char> KEYUP = { 27, 91, 65 }; 
const std::vector<char> KEYDOWN = { 27, 91, 66 }; 
const std::vector<char> KEYLEFT = { 27, 91, 68 }; 
const std::vector<char> KEYRIGHT = { 27, 91, 67 }; 

EMCScreen* emcScreen_ = NULL;

bool isKeystroke(std::vector<char>& keystroke, const std::vector<char>& k) {
    if (k.size() != keystroke.size()) {
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
    start = now();
    cbm64 = new CBM64Main();
    cbm64->Init();

    //Set the hires time callbacks
    hiresTime_ = new HiresTimeImpl();
    cbm64->SetHiresTimeProvider(hiresTime_);

    //Subscribe Host hardware related VIC Code
    emcScreen_ = new EMCScreen();
    CMOS6569* vic = cbm64->GetVic();
    unsigned char* screenBuffer = vic->RegisterHWScreen(emcScreen_);

    signal(SIGINT, Inthandler);

    std::thread t1([=]{
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

    std::thread t2([=]{
        printf("\e[?25l"); //cursor off
        while(_run) {
            uiloop();
            usleep(100000);
        }
        printf("\e[?25h"); //cursor on
    });


    std::cout << "\033c" << std::endl;

    runloop();

    std::cout << "press a key to end..." << std::endl;

    t1.join();
    t2.join();

    std::cout << "ended..." << std::endl;



}


