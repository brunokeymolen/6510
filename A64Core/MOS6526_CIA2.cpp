/*
 *  MOS6526.cpp
 *  A64Mac
 *
 *  Created by Bruno Keymolen on 13/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

//Serial:
//  The logic is inverted: 5V is 0 (false), and 0V is 1 (true).


#include "MOS6526_CIA2.h"


CMOS6526CIA2::CMOS6526CIA2(BKE_MUTEX mutex, C1541* disk){
	mMutex = mutex;
    m1541 = disk;
	mBus = CBus::GetInstance();
	mBus->Register(eBusCia2,this, 0xDD00, 0xDDFF);
}

CMOS6526CIA2::~CMOS6526CIA2(){
}

u8 CMOS6526CIA2::GetDeviceID(){
	return eBusCia2;
}

void CMOS6526CIA2::Cycle(uint64_t totalCycles){
    //Put the DATA line to TRUE to signal we are listening
    //setReg(0, PIN_DATA);
}

u8 CMOS6526CIA2::Peek(u16 address){
    //std::cout << "CIA2 PEEK ****************************" << std::endl;
	u8 val = mBus->PeekDevice(eBusRam,address);
#if 0
	if (address == 0xDD00) {
        std::cerr << " "
            << " ATN:  " << ((val & 8) > 0) 
            << " CLK:  " << ((val & 16) > 0)
            << " DATA: " << ((val & 32) > 0) << std::endl;
    }
#endif
    return val;
}

//https://www.c64-wiki.com/wiki/CIA
#define PINS_VIC_MEMORY 0x03
#define USERPORT_DATA 0x04
//Bit 3..5: serial bus Output (0=High/Inactive, 1=Low/Active)
#define PIN_ATN 0x08
#define PIN_CLK_OUT 0x10
#define PIN_DATA_OUT 0x20
//Bit 6..7: serial bus Input (0=Low/Active, 1=High/Inactive)
#define PIN_CLK_IN 0x40
#define PIN_DATA_IN 0x80

int CMOS6526CIA2::Poke(u16 address, u8 val){
    
    //std::cout << "CIA2 POKE **************************** " << std::hex << address  << " : " << (int)val << std::dec << std::endl;

	u16 reg = address - 0xDD00;
    u8 currentRegVal = mBus->PeekDevice(eBusRam, address);
   
    if (reg == 0) {
        std::cerr << std::endl;
        std::cerr << "\033[3;40f" << "* current; " 
            << "(" << std::hex << (int)currentRegVal << std::dec << ")"
            << "  ATN: " << ((currentRegVal & PIN_ATN) > 0) 
            << "  CLK_OUT: " << ((currentRegVal & PIN_CLK_OUT) > 0)
            << "  DATA_OUT: " << ((currentRegVal & PIN_DATA_OUT) > 0) 
            << "  CLK_IN: " << ((currentRegVal & PIN_CLK_IN) > 0)
            << "  DATA_IN: " << ((currentRegVal & PIN_DATA_IN) > 0) 
            << std::endl;
        std::cerr << "\033[4;40f" << "  new    ; "
            << "(" << std::hex << (int)val << std::dec << ")"
            << "  ATN: " << ((val & PIN_ATN) > 0) 
            << "  CLK_OUT: " << ((val & PIN_CLK_OUT) > 0)
            << "  DATA_OUT: " << ((val & PIN_DATA_OUT) > 0) 
            << "  CLK_IN: " << ((val & PIN_CLK_IN) > 0)
            << "  DATA_IN: " << ((val & PIN_DATA_IN) > 0) 
            << std::endl;
    }

    //protocol:
    // https://en.wikipedia.org/wiki/Commodore_bus
    // https://www.youtube.com/watch?v=_1jXExwse08&t=1693s
    
    //Registers:
    // https://www.c64-wiki.com/wiki/CIA

#if 0 
    std::cerr << "reg = " << reg << " currentRegVal: " << (int)currentRegVal << " val: " << (int)val << std::endl; 
    std::cerr << "val: " << (val & 8) << "  (currentRegVal & 8): " << (currentRegVal & 8) << std::endl; 
#endif    
    if ( reg ==  0) {
        if (((currentRegVal & PIN_ATN) == 0) && ((val & PIN_ATN) == PIN_ATN)) {
            std::cerr << "ATN went HI" << std::endl;
            std::cerr << "\033[1;30f" << "Serial ATN In/Out. Set low by the host (C64) to indicate the beginning of a serial data transfer.[9]" << std::endl;
            //sleep(3);
            m1541->SerialEvent(SerialPin::ATN, 1);
        } else if (((currentRegVal & PIN_ATN) == PIN_ATN) && ((val & PIN_ATN) == 0)) {
            m1541->SerialEvent(SerialPin::ATN, 0);
            std::cerr << "ATN went LO" << std::endl;
        } else if (((currentRegVal & PIN_CLK_IN) == 0) && ((val & PIN_CLK_IN) == PIN_CLK_IN)) {
            m1541->SerialEvent(SerialPin::CLK, 1);
            std::cerr << "CLK went HI" << std::endl;
        } else if (((currentRegVal & PIN_CLK_IN) == PIN_CLK_IN) && ((val & PIN_CLK_IN) == 0)) {
            m1541->SerialEvent(SerialPin::CLK, 0);
            std::cerr << "CLK went LO" << std::endl;
        } else if (((currentRegVal & PIN_DATA_IN) == 0) && ((val & PIN_DATA_IN) == PIN_DATA_IN)) {
            m1541->SerialEvent(SerialPin::DATA, 1);
            std::cerr << "DATA went HI" << std::endl;
        } else if (((currentRegVal & PIN_DATA_IN) == PIN_DATA_IN) && ((val & PIN_DATA_IN) == 0)) {
            std::cerr << "DATA went LO" << std::endl;
            m1541->SerialEvent(SerialPin::DATA, 0);
        }
    }

    mBus->PokeDevice(eBusRam,address,val);
	return 0;
}	

int CMOS6526CIA2::AddKeyStroke(char c){
	BKE_MUTEX_LOCK(mMutex);
	
	u8 bufPos = mBus->Peek(0xC6);
	if(bufPos >= 10){
		return -1;
	}
	mBus->PokeDevice(eBusRam, 0x0277+bufPos, c);
	mBus->Poke(0xC6, bufPos+1);
	
	BKE_MUTEX_UNLOCK(mMutex);

	return 0;
}


//SerialInterface
void CMOS6526CIA2::SetPin(SerialPin pin, u8 hilo) {
    std::cerr << "set serial pin " << (int)pin << " : " << (int)hilo << std::endl;
	u16 address = 0xDD00 + 0; //Register 0
    u8 val = mBus->PeekDevice(eBusRam, address);
    u8 bit = 0xff;
    switch (pin) {
        case SerialPin::ATN:
            bit = PIN_ATN; 
            break;
        case SerialPin::CLK:
            bit = PIN_CLK_IN; // (PIN_CLK_OUT | PIN_CLK_IN); 
            break;
        case SerialPin::DATA:
            bit = PIN_DATA_IN; // (PIN_DATA_OUT | PIN_DATA_IN);  
            break;
    }
    if (bit == 0xff) {
        std::cerr << "wrong pin id" << std::endl;
        return;
    }
    if (hilo == 0) {
        val = (val & (~bit));
    } else {
        val = (val | bit);
    }
    mBus->PokeDevice(eBusRam, address, val);
    
    std::cerr << "poke " << std::hex << address << " : " << (int)val << std::endl;
}

u8 CMOS6526CIA2::GetPin(SerialPin pin) {
	u16 address = 0xDD00 + 0; //Register 0
    u8 val = mBus->PeekDevice(eBusRam, address);
    u8 bit = 0xff;
    switch (pin) {
        case SerialPin::ATN:
            bit = PIN_ATN; 
            break;
        case SerialPin::CLK:
            bit = PIN_CLK_IN; 
            break;
        case SerialPin::DATA:
            bit = PIN_DATA_IN;  
            break;
    }
    if (bit == 0xff) {
        return 0;
    }

    return ((val & bit) > 0)?1:0; 
}



