/*
 *  Util.h
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 07/06/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
 
#include "Common.h"
#include "Bus.h"

#ifndef UTIL_H
#define UTIL_H

class CUtil{
private:
protected:
public:
	static void HexDumpMemory(CBus* mem, u16 address, u16 len); 
	static void HexDumpMemory(CBus* mem, u16 address, u16 len, int width); 
	static u8 Reverse(u8 n);
};

#endif //UTIL_H