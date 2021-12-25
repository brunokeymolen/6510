/*
 *  KernalRom.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 10/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "KernalRom.h"
#include "Bus.h"

#include <iostream>
#include <fstream>
#include "General.h"

CKernalRom::CKernalRom():CDevice(){
	//Load ROM's
	ifstream file(LOCATION_ROMS "KERNAL.ROM", ios::in|ios::binary|ios::ate);
	if (file.is_open()){
		file.seekg (0, ios::beg);
		file.read ((char*)mRom, KERNALROMSIZE);
		file.close();
	}else{
		cout << "Could not load KERNAL.ROM error" << endl;
		exit(-1);
	}
	
	CBus::GetInstance()->Register(eBusKernalRom, this, KERNALROMSTART, KERNALROMEND);

}




u8 CKernalRom::GetDeviceID(){
	return eBusKernalRom;
}

u8 CKernalRom::Peek(u16 address){
	return *(mRom+(address - KERNALROMSTART));
}

int CKernalRom::Poke(u16 address, u8 val){
	//Can not poke into ROM
	return -1;
}
