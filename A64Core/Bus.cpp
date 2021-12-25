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

CBus* CBus::_instance = 0;

CBus* CBus::GetInstance(){
	if(_instance == NULL){
		_instance = new CBus();
	}
	return _instance;
}


CBus::CBus(){
	//mDeviceCache = NULL;
	memset(&mIO, 0, sizeof(sBusDevice));
	memset(&mVic, 0, sizeof(sBusDevice));
	memset(&mCharRom, 0, sizeof(sBusDevice));
	memset(&mRam, 0, sizeof(sBusDevice));
	memset(&mBasicRom, 0, sizeof(sBusDevice));
	memset(&mKernalRom, 0, sizeof(sBusDevice));
	memset(&mCia1, 0, sizeof(sBusDevice));

	mLoRam = true;
	mHiRam = true;
	mCharen = true;

	//IO Ports, for simplification the ports are put here instead of at the processor
	mPort0 = 0xFF;
	mPort1 = 0xFF;

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
		default:
			cout << "Error! 109" << endl;
			break;
	}

}


void CBus::SetMode(e_BusMode mode){
	mMemoryMode = mode;
}	


u8 CBus::Peek(u16 address){	
	switch(mMemoryMode){
		case eBusModeProcesor:

			/* Basic ROM */
			if( address >= 0xA000 && address <= 0xBFFF ){
				if(mHiRam && mLoRam){
//cout << "Basic ROM" << endl;
					return mBasicRom.device->Peek(address);
				}else{
					return mRam.device->Peek(address);
				}
			}
			
			/* CharRom or IO */
			if( address >=0xD000 && address <= 0xDFFF ){
				if(mHiRam || mLoRam){
					if(mCharen){
//cout << "IO" << edl;
						return mIO.device->Peek(address);
					}else{
//cout << "CharRom." << endl;
						return mCharRom.device->Peek(address);
					}
				}else{
					return mRam.device->Peek(address);
				}
			 }
			 
			/* Kernal ROM */	
			if(address >= 0xE000 && address <= 0xFFFF){
				if(mHiRam){
//cout << "KernalRom." << endl;
					return mKernalRom.device->Peek(address);
				}else{
					return mRam.device->Peek(address);
				}
			}
			
			/* Byte 0 (Processor Port) */
			if( address == 0x0000 ){
//cout << "Byte 0" << endl;
				return 0x2F;
			}
		
				
			/* --- CHECK BELOW --- */	
			/* VIC IO - CHANGE THIS*/
			if(address >= mVic.fromAddress && address <= mVic.toAddress){
//cout << "VIC." << endl;
				return mVic.device->Peek(address);
			}
			
			/*  */
//			}else if(address >= mCia1.fromAddress && address <= mCia1.toAddress){
//cout << "CIA1" << endl;
//				return mCia1.device->Peek(address);
			/* --------------------*/
			
			/* RAM */
//cout << "RAM" << endl;
			return mRam.device->Peek(address);
			
			break;
		case eBusModeVic:
			SetMode(eBusModeProcesor);
			if(address >= 0x1000 && address <= 0x1FFF){
				address = (0xD000 + (address - 0x1000));
				return mCharRom.device->Peek(address);
			}else if(address >= 0x9000 && address <= 0x9FFF){
				address = (0xD000 + (address - 0x9000));
				return mCharRom.device->Peek(address);
			}
			return mRam.device->Peek(address);
		}				
	return 0;	
}


void CBus::Poke(u16 address, u8 m){
	switch(mMemoryMode){
		case eBusModeProcesor:
		/*	if(address >= mCharRom.fromAddress && address <= mCharRom.toAddress){
				 mCharRom.device->Poke(address,m);
			}else*/ 
			
			if(address == 0x0000 || address == 0x0001){
	//			cout << "WRITE 0x" << hex << (int)address << " val=0x" << (int)m << ", port0=" << (int)mPort0 << ", port1=" << (int)mPort1 << endl;
			}
			if(address == 0x0001){
				//IO Port
				{
					u8 r0 = mPort1; //mRam.device->Peek(1);

	//				cout << "WRITE 0x0001 : " << hex << (int)m << " & " << (int)mPort1 << " (" << (int)r0 << ")";
					
					u8 ioMem = m;// (r0 & m);
					mPort1 = ioMem;
					
					mLoRam = mHiRam = mCharen = 0;
					if(ioMem & 1)mLoRam = 1;
					if(ioMem & 2)mHiRam = 1;
					if(ioMem & 4)mCharen = 1;
					
					mRam.device->Poke(1,m);
					
//					cout  << " = " << hex << (int)ioMem << endl;
				}
//			}else if( address >=0xD000 && address <= 0xDFFF ){
//				mIO.device->Poke(address,m);

			}else if(address >= mIO.fromAddress && address <= mIO.toAddress){
				 mIO.device->Poke(address,m);
			}else if(address >= mVic.fromAddress && address <= mVic.toAddress){
				 mVic.device->Poke(address,m);
			}else if(address >= mBasicRom.fromAddress && address <= mBasicRom.toAddress){
				 mBasicRom.device->Poke(address,m);
			}else if(address >= mKernalRom.fromAddress && address <= mKernalRom.toAddress){
				 mKernalRom.device->Poke(address,m);
			}else if(address >= mCia1.fromAddress && address <= mCia1.toAddress){
				 mCia1.device->Poke(address,m);
			}else if(address >= mRam.fromAddress && address <= mRam.toAddress){
				 mRam.device->Poke(address,m);
			}else{
				cout << "Error 822" << endl;
			}
			break;
		case eBusModeVic:
			SetMode(eBusModeProcesor);	
			mRam.device->Poke(address, m);
			break;
		}
}


u16 CBus::Peek16(u16 address){	
	u16 ret = 0;
	ret = Peek(address+1) << 8;
	ret = ret | Peek(address);
//	((u8*)&ret)[0] = Peek(address);
//	((u8*)&ret)[1] = Peek(address+1);
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
		default:
			cout << "Error! 1239" << endl;
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
		default:
			cout << "Error! 1239" << endl;
			break;
	}
	return 0;
}

