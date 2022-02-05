/*
 *  KernalRom.h
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 10/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef KERNALROM_H
#define KERNALROM_H

#include "Common.h"
#include "Device.h"

#define KERNALROMSTART 0xE000 
#define KERNALROMEND   0xFFFF 
#define KERNALROMSIZE  (KERNALROMEND - KERNALROMSTART + 1) 

class CKernalRom : public CDevice{
	private:
		u8 mRom[KERNALROMSIZE];
		CKernalRom& operator+=(CKernalRom&);
		
	protected:
		void GetDefaultAddressRange(u16 *from, u16 *to);
	public:
		CKernalRom();
		
		u8 GetDeviceID();
		u8 Peek(u16 address);
		int Poke(u16 address, u8 val); 
	
};

#endif //KERNALROM_H
