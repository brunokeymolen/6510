/*
 *  Ram.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 10/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Ram.h"
#include "Bus.h"

#include <iostream>
#include <fstream>

CRam::CRam():CDevice(){
	CBus::GetInstance()->Register(eBusRam, this, 0x0000, 0xFFFF);

	memset(mRam, 0, MAXRAM);
	memset(mRam + 0xD000, 0, 0xDFFF-0xD000);
}


u8 CRam::GetDeviceID(){
	return eBusRam;
}

u8 CRam::Peek(u16 address){
	return *(mRam+address);
}

int CRam::Poke(u16 address, u8 val){
	*(mRam+address) = val;
	return 0;
}

int CRam::LoadBasic(char* fname){
	u8* m;
	u16 a;
	m = mRam+0x0800;
	ifstream file(fname, ios::in|ios::binary|ios::ate);
	if (file.is_open()){
		file.seekg (0, ios::beg);
		file.read((char*)&a,2); //ignore when basic
		file.read ((char*)m, (0x9FFF - 0x0800)); //Basic area : 0x0800 - 0x9FFF
		a = 0x0800 + file.gcount(); //basic
		a = a + file.gcount();
		file.close();
		CBus::GetInstance()->Poke16(0x002D, a);
	}else{
		cout << "Could not load file : " << fname << endl;
		return -1;
	}

	return 0;
}

int CRam::LoadApp(char* fname){
	u8* m;
	u16 a;
	ifstream file(fname, ios::in|ios::binary|ios::ate);
	if (file.is_open()){
		file.seekg (0, ios::beg);
		file.read((char*)&a,2); 
		m = mRam+a; 	
		file.read ((char*)m, (0x9FFF - a)); //Basic area : 0x0800 - 0x9FFF
		a = a + file.gcount();
		file.close();
		CBus::GetInstance()->Poke16(0x002D, a);
	}else{
		cout << "Could not load file : " << fname << endl;
		return -1;
	}

	return 0;
}
