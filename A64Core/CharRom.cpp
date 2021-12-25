/*
 *  CharRom.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 10/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "CharRom.h"
#include <iostream>
#include <fstream>
#include "Bus.h"
#include "General.h"

CCharRom::CCharRom():CDevice(){
	//Load ROM's
	ifstream file (LOCATION_ROMS "CHAR.ROM", ios::in|ios::binary|ios::ate);
	if (file.is_open()){
		file.seekg (0, ios::beg);
		file.read ((char*)mRom, CHARROMSIZE);
		file.close();
	}else{
		cout << "Could not load CHAR.ROM error" << endl;
		exit(-1);
	}
	
	CBus::GetInstance()->Register(eBusCharRom, this, CHARROMSTART, CHARROMEND);
	
}



u8 CCharRom::GetDeviceID(){
	return eBusCharRom;
}

u8 CCharRom::Peek(u16 address){
//cout << "CCharRom Peek:" << (int)address << endl;

	return *(mRom+(address - CHARROMSTART));
}

int CCharRom::Poke(u16 address, u8 val){
	//Can not poke into ROM
	cout << "CCharRom Poke:" << (int)address << ", m=" << (int)val << endl;
	CBus::GetInstance()->PokeDevice(eBusRam,address,val);

	return -1;
}
