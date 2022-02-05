/*
 *  MOS6569.h 
 *  A64Emulator
 *  
 *  6569 (PAL) Video Chip (VIC II) Emulator  
 *
 *  Created by Bruno Keymolen on 06/07/08.
 *  Copyright 2008 Bruno Keymolen. All rights reserved.
 *
 */

#ifndef MOS6569_H
#define MOS6569_H

#include "Common.h"
#include "Device.h"
#include "Bus.h"

#define VICRES_X 320
#define VICRES_Y 200

typedef struct _VICRect{
	u16 x;
	u16 y;
	u16 width;
	u16 height;
}VICRect;



class CVICHWScreen{
public:
		virtual void DrawPixels(u8* screenBuffer, VICRect* area) = 0; //screenbuffer = 320x200
		virtual void DrawChar(u16 address, u8 c)  = 0;
		virtual void DrawChars(u8* memory) = 0;
};


class CMOS6569 : public CDevice{
	private:
		CVICHWScreen* mRenderer;
		u8 mVideoMem[0x0800-0x0400];
		u8 mScreenBufPixel[ (VICRES_X /8) * VICRES_Y + VICRES_X ];
		CBus* mBus;	
	protected:
	public:
		CMOS6569();
		u8 GetDeviceID();

        void Cycle();

		u8 Peek(u16 address);
		int Poke(u16 address, u8 val); 

		void SetChar(u16 address, u8 c); //temp
		
		//Hardware dependend calls
		u8* RegisterHWScreen(CVICHWScreen* screen);
		void HWNeedsRedraw();
};




#endif //MOS6569_H
