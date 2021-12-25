//
//  C64CocoaInit.mm
//  A64Mac
//
//  Created by Bruno Keymolen on 12/07/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "C64Cocoa.h"
#include "A64Core/CBM64Main.h"
#include "A64Core/MOS6569.h"
#include "VICView.h"

class CVICObserver: public CVICHWScreen, public CHiresTime{
private:
	VICView* mVicView;
public:
	CVICObserver(VICView* vicView){
		mVicView = vicView;
	}
	
	void DrawChar(unsigned short address, unsigned char c){
		[mVicView DrawCharAt:address character:c];
	}
		
	void DrawChars(u8* memory){
		[mVicView DrawCharsFromMemory:memory];
	}
	
	//screenbuffer = 320x200
	void DrawPixels(unsigned char* screenBuffer, VICRect* area){
		[mVicView DrawPixelsFromBuffer:screenBuffer X:area->x Y:area->y Width:area->width Height:area->height];
	}
	
	double GetMicroseconds(){
		UnsignedWide currentTime;
		Microseconds(&currentTime);
		return (currentTime.hi *  4294967296.0) + currentTime.lo; 
	}
	
	int GetMicrosecondsLo(){
//		static UnsignedWide currentTime;
//		Microseconds(&currentTime);
//		return currentTime.lo;
		
//		static AbsoluteTime t;
//		t = UpTime();		
//		return AbsoluteToDuration(t)*1000;
		
		static timeval t;
		gettimeofday(&t, NULL);	
		return ((t.tv_sec * 1000000) + t.tv_usec);
	}

	 
};

@implementation C64Cocoa


-(void)InitWithView:(NSView*)view{
	VICView* vicView = (VICView*)view;
	
	CVICObserver* vicObserver = new CVICObserver(view);

	mCBM64 = new CBM64Main();
	((CBM64Main*)mCBM64)->Init();

	
	//Set the hires time callbacks
	((CBM64Main*)mCBM64)->SetHiresTimeProvider(vicObserver);


	//Subscribe Host hardware related VIC Code
	CMOS6569* vic = ((CBM64Main*)mCBM64)->GetVic();
	unsigned char* screenBuffer = vic->RegisterHWScreen(vicObserver);
	[vicView SetScreenBuffer:screenBuffer];
	
	//Subscribe Host hardware related SID Code
	

}


-(void)Start{
	((CBM64Main*)mCBM64)->Run();
}

-(int)AddKeyStroke:(char)c{
	if(c == ']'){
//		((CBM64Main*)mCBM64)->LoadBasic("/Users/bruno/Projects/A64/Data/sin.prg");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Even/MONOPOLY.PRG");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Even/Panic_Express.prg");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Even/Goldrunner.prg");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Projects/A64/Data/ED64doc Build an Emulator/working/monopoly (n).prg");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Projects/A64/Data/ED64doc Build an Emulator/working/ADVENTURE.PRG");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Projects/A64/Data/ED64doc Build an Emulator/working/blackjack v3.prg");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Projects/A64/Data/ED64doc Build an Emulator/working/match.prg");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Projects/A64/Data/ED64doc Build an Emulator/working/match (colour).prg");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Projects/A64/Data/ED64doc Build an Emulator/working/falcon quest.prg");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Projects/A64/Data/ED64doc Build an Emulator/working/match (colour).prg");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Projects/A64/GameRoms/P/Partygirls.p00");
//		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/Projects/A64/GameRoms/#/1942/1942_Elite\ Systems.PRG");

		((CBM64Main*)mCBM64)->LoadApp("/Users/bruno/even/neural.prg");
		 
		
		
	}else if(c == '['){
		((CBM64Main*)mCBM64)->SetDisassemble(0);	
	}else if(c == '<'){
		int d = ((CBM64Main*)mCBM64)->GetDisassemble();
		((CBM64Main*)mCBM64)->SetDisassemble(d + 100);	
	}else if(c == '>'){
		int d = ((CBM64Main*)mCBM64)->GetDisassemble();
		d=d-100;
		if(d<30)d=30;	
		((CBM64Main*)mCBM64)->SetDisassemble(d);
	}else{
	//	if(c == 13)((CBM64Main*)mCBM64)->SetDisassemble(1);
		((CBM64Main*)mCBM64)->GetCia1()->AddKeyStroke(c);
	}
	return 0;
}


@end
