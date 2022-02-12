/*
 *  MOS6569.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 06/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

//https://www.cebix.net/VIC-Article.txt

#include "MOS6569.h"
#include "Util.h"



static char CMOS6569TextMap[128] = 
				 {    '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
					  'P','Q','R','S','T','U','V','W','X','Y','Z','[','x',']','^','x',
                      ' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/',
                      '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
                      'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x',
                      'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x',
                      ' ','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x',
                      'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x' };


CMOS6569::CMOS6569(){
	memset(mVideoMem, 44, (0x0800-0x0400));
	mBus = CBus::GetInstance(); 
	mBus->Register(eBusVic, this, 0x0400, 0x07FF);
	mBus->Register(eBusIO, this, 0xD000, 0xDFFF); //@TODO, should be devided over other IO devices
}

void CMOS6569::SetChar(u16 address, u8 c){
}

u8 CMOS6569::GetDeviceID(){
	return eBusVic;
}

void CMOS6569::Cycle(uint64_t totalCycles){
}

u8 CMOS6569::Peek(u16 address){
    //std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>> Peek (mos6596) address: " << std::hex << address << std::endl;
    if(address >= 0x0400 && address <= 0x07FF){
		return mVideoMem[address-0x0400];
	}
	if(address == 0xD012){
		return 0;
	}
	return 0xff;
}


int CMOS6569::Poke(u16 address, u8 val){
    //std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Poke (mos6596) address: " << std::hex << address << std::endl;
	VICRect rect;
	u8 v; //Vertical (Y) position of char
	u8 cl; //character line
	int p;
	
	
	//In character buffer 40x25?
	if(address >= 0x0400 && address <= 0x07FF){
		//adress-0x0400
		mVideoMem[address-0x0400] = val;
		
		//Write it to our screen buffer (320 * 200)
		
		rect.x = (address-0x0400)%40;//= p*8
		rect.y = (u16) ((address-0x0400)/40)*8;
		p = rect.x + (rect.y * 40);

		for(v=0;v<8;v++){
			mBus->SetMode(eBusModeVic);
			//Read char from charRom (from VIC address mode charrom = 0x1000 or 0x9000)
			cl = mBus->Peek(0x1000 + (val * 8) + v);
			
			mScreenBufPixel[ /*Y=*/   /*X=*/  p + (v * 40 /*(320/8)*/)] = CUtil::Reverse(cl);
		}
		
		rect.width = 8;
		rect.height = 8;
		mRenderer->DrawPixels(mScreenBufPixel, &rect);		
		mRenderer->DrawChars(mVideoMem);
	}
	return 0;
}


u8* CMOS6569::RegisterHWScreen(CVICHWScreen* screen){
	mRenderer = screen;
	return mScreenBufPixel;
}


void CMOS6569::HWNeedsRedraw(){
}

