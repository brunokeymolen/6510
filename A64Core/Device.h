/*
 *  Device.h
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 10/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DEVICE_H
#define DEVICE_H

#include "Common.h"

class CDevice{
private:
protected:
public:
	CDevice();
	~CDevice();
//	virtual u16 GetFromAddress() = 0;
//	virtual u16 GetToAddress() = 0;
	virtual u8 GetDeviceID() = 0;
	virtual u8 Peek(u16 address) = 0;
	virtual int Poke(u16 address, u8 val) = 0; 
};

#endif //DEVICE_H