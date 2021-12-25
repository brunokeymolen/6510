/*
 *  Util.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 07/06/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Util.h"

void CUtil::HexDumpMemory(CBus* mem, u16 address, u16 len, int width){
	address = address - (address % width); 
	cout << hex << setfill('0') << uppercase;
	for(int mc=address; mc<address+len; mc++){
		if((mc-address) % width == 0){
			cout << endl << "0x" << setw(4) << mc << " : ";  
		}
		cout << " 0x" << setw(2) << (int)mem->Peek(mc);
	}
	cout << nouppercase << endl;
}

void CUtil::HexDumpMemory(CBus* mem, u16 address, u16 len){
	HexDumpMemory(mem, address, len, 8);
}


u8 CUtil::Reverse(u8 n)
{
  return ((n & 0x01) << 7)
  | ((n & 0x02) << 5)
  | ((n & 0x04) << 3)
  | ((n & 0x08) << 1)
  | ((n & 0x10) >> 1)
  | ((n & 0x20) >> 3)
  | ((n & 0x40) >> 5)
  | ((n & 0x80) >> 7);
} 