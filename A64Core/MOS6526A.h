/*
 *  MOS6526.h
 *  A64Mac
 *  
 *  CIA 6526 Complex Interface Adapter Emulator  
 *
 *  Created by Bruno Keymolen on 13/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

// https://www.c64-wiki.com/wiki/CIA

#ifndef MOS6569A_H
#define MOS6569A_H

#include "Common.h"
#include "Device.h"
#include "Bus.h"

class CMOS6526A : public CDevice{
private:
	CBus* mBus;
	BKE_MUTEX mMutex;
protected:
public:
	CMOS6526A(BKE_MUTEX mutex);
	~CMOS6526A();

    void Tick();

	u8 GetDeviceID();
	u8 Peek(u16 address);
	int Poke(u16 address, u8 val); 	

	int AddKeyStroke(char c);
};


#endif //MOS6569A_H
