/*
 *  MOS6526.cpp
 *  A64Mac
 *
 *  Created by Bruno Keymolen on 13/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "MOS6526_CIA1.h"


CMOS6526CIA1::CMOS6526CIA1(BKE_MUTEX mutex){
	mMutex = mutex;
	mBus = CBus::GetInstance();
	mBus->Register(eBusCia1,this, 0xDC00, 0xDCFF);
}

CMOS6526CIA1::~CMOS6526CIA1(){
}

u8 CMOS6526CIA1::GetDeviceID(){
	return eBusCia1;
}

void CMOS6526CIA1::Cycle(uint64_t totalCycles){
}

u8 CMOS6526CIA1::Peek(u16 address){
    u8 val = mBus->PeekDevice(eBusRam,address);
    //std::cout << "CIA1 PEEK **************************** " << std::hex << address  << " : " << (int)val << std::dec << std::endl;
    return val;
}


int CMOS6526CIA1::Poke(u16 address, u8 val){
    //std::cout << "CIA1 POKE **************************** " << std::hex << address  << " : " << (int)val << std::dec << std::endl;
	mBus->PokeDevice(eBusRam,address,val);
	return 0;
}	

int CMOS6526CIA1::AddKeyStroke(char c){
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


