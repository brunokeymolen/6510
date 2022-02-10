/*
 *  Bus.h
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 10/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef BUS_H
#define BUS_H

#include "Common.h"
#include "Device.h"

#include <vector>

typedef struct _sBusDevice{
	CDevice* device;
	u16 fromAddress;
	u16 toAddress;
}sBusDevice;

typedef enum _e_BusDevice{
	eBusRam = 1,
	eBusBasicRom,
	eBusKernalRom,
	eBusSid,
	eBusVic,
	eBusCia1,
	eBusCia2,
	eBusIO,
	eBusCharRom
}e_BusDevice;

typedef enum _e_BusMode{
	eBusModeProcesor = 0,
	eBusModeVic
}e_BusMode;


class CBus{
private:
	static CBus* _instance;
	
	sBusDevice mVic; //Video Chip, VIC-II (6569)
	sBusDevice mRam;
	sBusDevice mBasicRom;
	sBusDevice mKernalRom;
	sBusDevice mProcessor;
	sBusDevice mCia1;
	sBusDevice mCia2;
	sBusDevice mCharRom;
	sBusDevice mIO;

	e_BusMode mMemoryMode;
	
	u8 mPort0;
	u8 mPort1;

	bool mLoRam;
	bool mHiRam;
	bool mCharen;
protected:
	CBus();
public:
	static CBus* GetInstance();	
	void Register(e_BusDevice devid, CDevice* device, u16 fromAddress, u16 toAddress);

	u8 Peek(u16 address);
	u16 Peek16(u16 address);
	void Poke(u16 address, u8 m);
	void Poke16(u16 address, u16 m);

	void PokeDevice(u8 deviceID, u16 address, u8 m);
	u8 PeekDevice(u8 deviceID, u16 address);
	void SetMode(e_BusMode mode);

};


#endif
