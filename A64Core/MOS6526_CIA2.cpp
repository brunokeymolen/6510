/*
 *  MOS6526.cpp
 *  A64Mac
 *
 *  Created by Bruno Keymolen on 13/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "MOS6526_CIA2.h"


CMOS6526CIA2::CMOS6526CIA2(BKE_MUTEX mutex){
	mMutex = mutex;
	mBus = CBus::GetInstance();
	mBus->Register(eBusCia2,this, 0xDD00, 0xDDFF);
}

CMOS6526CIA2::~CMOS6526CIA2(){
}

u8 CMOS6526CIA2::GetDeviceID(){
	return eBusCia2;
}

void CMOS6526CIA2::Cycle(uint64_t totalCycles){
}

u8 CMOS6526CIA2::Peek(u16 address){
    //std::cout << "CIA2 PEEK ****************************" << std::endl;
    return 3;
	return mBus->PeekDevice(eBusRam,address);
}


int CMOS6526CIA2::Poke(u16 address, u8 val){
    //std::cout << "CIA2 POKE **************************** " << std::hex << address  << " : " << (int)val << std::dec << std::endl;
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


