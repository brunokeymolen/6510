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

#ifndef MOS6569CIA2_H
#define MOS6569CIA2_H

#include "Common.h"
#include "Device.h"
#include "Bus.h"
#include "1541.h"
#include "serial.h"

class CMOS6526CIA2 : public CDevice, public SerialInterface {
private:
	CBus* mBus;
    C1541* m1541;
	BKE_MUTEX mMutex;
protected:
public:
	CMOS6526CIA2(BKE_MUTEX mutex, C1541* disk);
	~CMOS6526CIA2();

    void Cycle(uint64_t totalCycles);

	u8 GetDeviceID();
	u8 Peek(u16 address);
	int Poke(u16 address, u8 val); 	

	int AddKeyStroke(char c);

public:
    //SerialInterface
    virtual void SetPin(SerialPin pin, u8 hilo) override;
    virtual u8 GetPin(SerialPin pin) override;
};


#endif //MOS6569A_H
