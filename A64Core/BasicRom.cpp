/*
 *  BasicRom.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 10/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "BasicRom.h"
#include <iostream>
#include <fstream>
#include "Bus.h"
#include "General.h"


#ifdef EMBEDDED_ROMS
extern char _binary_BASIC_ROM_start[];
extern char _binary_BASIC_ROM_end[];
#endif

CBasicRom::CBasicRom():CDevice(){
#ifdef EMBEDDED_ROMS
    std::cout << "adding embedded rom" << std::endl;
    int len = (uint64_t)(&_binary_BASIC_ROM_end) - (uint64_t)(&_binary_BASIC_ROM_start);
    if (BASICROMSIZE != len) {
        std::cout << "basic rom error... embedded len: " << len << " expect: " << BASICROMSIZE << std::endl;
        exit(-1);
    }
    memcpy(mRom, _binary_BASIC_ROM_start, BASICROMSIZE);
#else
	//Load ROM's
	ifstream file (LOCATION_ROMS "BASIC.ROM", ios::in|ios::binary|ios::ate);
	if (file.is_open()){
		file.seekg (0, ios::beg);
		file.read ((char*)mRom, BASICROMSIZE);
		file.close();
	}else{
		cout << "Could not load BASIC.ROM error" << endl;
		exit(-1);
	}
#endif	
	CBus::GetInstance()->Register(eBusBasicRom, this, BASICROMSTART, BASICROMEND);
	
}



u8 CBasicRom::GetDeviceID(){
	return eBusBasicRom;
}

u8 CBasicRom::Peek(u16 address){
//cout << "CBasicRom Peek:" << (int)address << endl;

//if(address - BASICROMSTART == 876){
//exit(-2);
//}
	return *(mRom+(address - BASICROMSTART));
}

int CBasicRom::Poke(u16 address, u8 val){
	//Can not poke into ROM
	//cout << "CBasicRom Poke:" << (int)address << ", m=" << (int)val << endl;
	CBus::GetInstance()->PokeDevice(eBusRam,address,val);

	return -1;
}
