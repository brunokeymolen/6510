/*
 *  BasicRom.h
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 10/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BASICROM_H
#define BASICROM_H

#include "Common.h"
#include "Device.h"

#define BASICROMSTART  0xA000
#define BASICROMEND    0xBFFF
#define BASICROMSIZE   (BASICROMEND - BASICROMSTART + 1) 

class CBasicRom : public CDevice{
	private:
		u8 mRom[BASICROMSIZE];
		CBasicRom& operator+=(CBasicRom&);
	//	CBasicRom(CBasicRom&);
	
	protected:
		void GetDefaultAddressRange(u16 *from, u16 *to);
	public:
		CBasicRom();
		
		u8 GetDeviceID();
		u8 Peek(u16 address);
		int Poke(u16 address, u8 val); 
	
};

#endif //BASICROM_H
