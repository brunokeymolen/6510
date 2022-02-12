/*
 *  Bus.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 10/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

/*
	Memory Banking
	
	0x0000 : Masked IO Register
				Bit 0:LORAM
				    1:HIRAM
					2:CHAREN
	0x0001 : IO Port
	
*/


#include "Bus.h"
#include "Common.h"

CBus* CBus::_instance = 0;

CBus* CBus::GetInstance(){
	if(_instance == NULL){
		_instance = new CBus();
	}
	return _instance;
}


CBus::CBus(){
	memset(&mIO, 0, sizeof(sBusDevice));
	memset(&mVic, 0, sizeof(sBusDevice));
	memset(&mCharRom, 0, sizeof(sBusDevice));
	memset(&mRam, 0, sizeof(sBusDevice));
	memset(&mBasicRom, 0, sizeof(sBusDevice));
	memset(&mKernalRom, 0, sizeof(sBusDevice));
	memset(&mCia1, 0, sizeof(sBusDevice));
	memset(&mCia2, 0, sizeof(sBusDevice));

	mLoRam = true;
	mHiRam = true;
	mCharen = true;

	mMemoryMode = eBusModeProcesor;
}


void CBus::Register(e_BusDevice devid, CDevice* device, u16 fromAddress, u16 toAddress){

	switch(devid){
		case eBusIO:
			mIO.device=device;
			mIO.fromAddress = fromAddress;
			mIO.toAddress = toAddress;
			break;		
		case eBusVic:
			mVic.device=device;
			mVic.fromAddress = fromAddress;
			mVic.toAddress = toAddress;
			break;
		case eBusCharRom:
			mCharRom.device=device;
			mCharRom.fromAddress = fromAddress;
			mCharRom.toAddress = toAddress;
			break;		
		case eBusRam:
			mRam.device=device;
			mRam.fromAddress = fromAddress;
			mRam.toAddress = toAddress;
            //control registers
   	        mLoRam = mHiRam = mCharen = true;
            mRam.device->Poke(0x0000, 0xff);
            mRam.device->Poke(0x0001, 0xff);
			break;
		case eBusBasicRom:
			mBasicRom.device=device;
			mBasicRom.fromAddress = fromAddress;
			mBasicRom.toAddress = toAddress;
			break;
		case eBusKernalRom:
			mKernalRom.device=device;
			mKernalRom.fromAddress = fromAddress;
			mKernalRom.toAddress = toAddress;
			break;
		case eBusCia1:
			mCia1.device=device;
			mCia1.fromAddress = fromAddress;
			mCia1.toAddress = toAddress;
			break;
		case eBusCia2:
			mCia2.device=device;
			mCia2.fromAddress = fromAddress;
			mCia2.toAddress = toAddress;
			break;
		default:
            std::cout << "Error! 109" << std::endl;
			break;
	}

}


void CBus::SetMode(e_BusMode mode){
	mMemoryMode = mode;
}	


u8 CBus::Peek2(u16 address){
    u8 val = Peek2(address);
    std::cerr << "Peek: " << std::hex << address << " : " << (int)val << std::endl;
    return val;
}


u8 CBus::Peek(u16 address){
    //https://www.c64-wiki.com/wiki/Bank_Switching
    if(address == 0x0000) {
        //0x0000 - Directional data register (bits 0,1,2)
        return mRam.device->Peek(0x0000);
    }else if(address == 0x0001){
        //0x0001 - Port register (bits 0,1,2)	
        return mRam.device->Peek(0x0001);
    } else if (address >= 0xE000 && address <= 0xFFFF) {
        //Kernal ROM / RAM / CHART ROM HI 
        if (mHiRam) {
            return mKernalRom.device->Peek(address);
        } else {
            return mRam.device->Peek(address);
        }
    } else if (address >= 0x0400 && address <= 0x07FF) {
        //Screen memory
        return mVic.device->Peek(address);
    } else if (address >= 0xD000 && address <= 0xDFFF) {
        //RAM / Char ROM / IO
        if (mCharen) {
            //IO
            if (address >= 0xDF00 && address <= 0xDFFF) {
                //IO 2
                return mRam.device->Peek(address);
            } else if (address >= 0xDE00 && address <= 0xDEFF) {
                //IO 1
                return mRam.device->Peek(address);
            } else if (address >= 0xDD00 && address <= 0xDDFF) {
                //CIA 2
                return mCia2.device->Peek(address);
            } else if (address >= 0xDC00 && address <= 0xDCFF) {
                //CIA 1
                return mCia1.device->Peek(address);
            } else if (address >= 0xD800 && address <= 0xDAFF) {
                //Color RAM
                return mRam.device->Peek(address);
            } else if (address >= 0xD400 && address <= 0xD7FF) {
                //SID
                return mRam.device->Peek(address);
            } else if (address >= 0xD000 && address <= 0xD3FF) {
                //VIC
                return mVic.device->Peek(address);
            } else {
                return mRam.device->Peek(address);
            }
        } else {
            if (mHiRam == 0 && mLoRam == 0) {
                //RAM
                return mRam.device->Peek(address);
            } else {
                //CHAR ROM
                return mCharRom.device->Peek(address);
            }
        }
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        //Basic ROM / RAM / CHART ROM HI 
        if (mLoRam && mHiRam) {
            return mBasicRom.device->Peek(address);
        } else {
            return mRam.device->Peek(address);
        }
    } else if (address >= 0x8000 && address <= 0x9FFF) {
        //Chart ROM LO
        return mRam.device->Peek(address);
    } else {
        return mRam.device->Peek(address);
    }
    return 0;
}

//https://www.c64-wiki.com/wiki/Bank_Switching
//https://www.c64-wiki.com/wiki/Memory_Map
void CBus::Poke(u16 address, u8 m){
    //std::cerr << "Poke: " << std::hex << address << " : " << (int)m<< std::endl;
			
    //https://www.c64-wiki.com/wiki/Bank_Switching
    if(address == 0x0000) {
        //0x0000 - Directional data register (bits 0,1,2)
#if 0
        u8 pr = mRam.device->Peek(0x0001);

        mLoRam = mHiRam = mCharen = 0;
        if ((m & 1) && (pr & 1)) mLoRam = 1;
        if ((m & 2) && (pr & 2)) mHiRam = 1;
        if ((m & 4) && (pr & 4)) mCharen = 1;
#endif     
        mRam.device->Poke(0x0000, m);
    
    }else if(address == 0x0001){
        //0x0001 - Port register (bits 0,1,2)	
        //u8 cr = mRam.device->Peek(0x0000);

        mLoRam = mHiRam = mCharen = 0;
        if ((m & 1)) mLoRam = 1;
        if ((m & 2)) mHiRam = 1;
        if ((m & 4)) mCharen = 1;
            
        mRam.device->Poke(0x0001, m);

    } else if (address >= 0xE000 && address <= 0xFFFF) {
        //Kernal ROM / RAM / CHART ROM HI 
        if (mHiRam) {
            mKernalRom.device->Poke(address,m);
        } else {
            mRam.device->Poke(address, m);
        }
    } else if (address >= 0x0400 && address <= 0x07FF) {
        //Screen memory
        mVic.device->Poke(address, m);
    } else if (address >= 0xD000 && address <= 0xDFFF) {
        //std::cout << "POKE " << std::hex << address << std::dec << " mCharen: " << mCharen << std::endl;
        //RAM / Char ROM / IO
        if (mCharen) {
            //IO
            if (address >= 0xDF00 && address <= 0xDFFF) {
                //IO 2
                mRam.device->Poke(address, m);
            } else if (address >= 0xDE00 && address <= 0xDEFF) {
                //IO 1
                mRam.device->Poke(address, m);
            } else if (address >= 0xDD00 && address <= 0xDDFF) {
                //CIA 2
                mCia2.device->Poke(address, m);
            } else if (address >= 0xDC00 && address <= 0xDCFF) {
                //CIA 1
                mCia1.device->Poke(address, m);
            } else if (address >= 0xD800 && address <= 0xDAFF) {
                //Color RAM
                mRam.device->Poke(address, m);
            } else if (address >= 0xD400 && address <= 0xD7FF) {
                //SID
                mRam.device->Poke(address, m);
            } else if (address >= 0xD000 && address <= 0xD3FF) {
                //VIC
                mVic.device->Poke(address, m);
            } else {
                mRam.device->Poke(address, m);
            }        
        } else {
            if (mHiRam == 0 && mLoRam == 0) {
                //RAM
                mRam.device->Poke(address, m);
            } else {
                //CHAR ROM
                mCharRom.device->Poke(address, m);
            }
        }
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        //Basic ROM / RAM / CHART ROM HI 
        if (mLoRam && mHiRam) {
            mBasicRom.device->Poke(address,m);
        } else {
            mRam.device->Poke(address, m);
        }
    } else if (address >= 0x8000 && address <= 0x9FFF) {
        //Chart ROM LO
        mRam.device->Poke(address, m);
    } else {
        mRam.device->Poke(address, m);
    }
}


u16 CBus::Peek16(u16 address){	
	u16 ret = 0;
	ret = Peek(address+1) << 8;
	ret = ret | Peek(address);
	return ret;
}

void CBus::Poke16(u16 address, u16 m){
	Poke(address, ((u8*)&m)[0]);
	Poke(address+1, ((u8*)&m)[1]);
}


void CBus::PokeDevice(u8 deviceID, u16 address, u8 m){
	
	switch(deviceID){
		case eBusIO:
			mIO.device->Poke(address, m);
			break;		
		case eBusVic:
			mVic.device->Poke(address, m);
			break;		
		case eBusRam:
			mRam.device->Poke(address, m);
			break;		
		case eBusBasicRom:
			mBasicRom.device->Poke(address, m);
			break;		
		case eBusKernalRom:
			mKernalRom.device->Poke(address, m);
			break;		
		case eBusCharRom:
			mCharRom.device->Poke(address, m);
			break;		
		case eBusCia1:
			mCia1.device->Poke(address, m);
			break;		
		case eBusCia2:
			mCia2.device->Poke(address, m);
			break;		
		default:
            std::cout << "Error! 1239" << std::endl;
			break;
	}
}

u8 CBus::PeekDevice(u8 deviceID, u16 address){
	switch(deviceID){
		case eBusIO:
			return mIO.device->Peek(address);
			break;		
		case eBusVic:
			return mVic.device->Peek(address);
			break;		
		case eBusRam:
			return mRam.device->Peek(address);
			break;		
		case eBusBasicRom:
			return mBasicRom.device->Peek(address);
			break;		
		case eBusKernalRom:
			return mKernalRom.device->Peek(address);
			break;		
		case eBusCharRom:
			return mCharRom.device->Peek(address);
			break;		
		case eBusCia1:
			return mCia1.device->Peek(address);
			break;		
		case eBusCia2:
			return mCia2.device->Peek(address);
			break;		
		default:
            std::cout << "Error! 1239" << std::endl;
			break;
	}
	return 0;
}

