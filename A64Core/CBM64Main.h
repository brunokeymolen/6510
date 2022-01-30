/*
 *  CBM64Main.h
 *  A64Mac
 *
 *  Created by Bruno Keymolen on 12/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CBM64MAIN_H
#define CBM64MAIN_H

#include <iostream>
//#include "UTestOpcode.h"
#include "MOS6510.h"
#include "MOS6569.h"
#include "Bus.h"
#include "Ram.h"
//#include "Memory.h"
#include "BasicRom.h"
#include "KernalRom.h"
#include "MOS6526A.h"
#include "CharRom.h"
#include "General.h"


class CBM64Main{
	private:
		CBus* mBus;
		CMOS6569* mVic; //Video Chip, VIC-II (6569)
		CRam* mRam;
		CBasicRom* mBasicRom;
		CKernalRom* mKernalRom;
		CCharRom* mCharRom;
		CMOS6510* mProcessor;
		CMOS6526A* mCia1;
		
		BKE_THREAD mCBM64Thread;
		BKE_MUTEX mMutex;
	protected:
	public:
		int Init();
		int Run();
		int Stop();

		int RunPriv();
		int SetDisassemble(int d);
		int GetDisassemble();
		
		CMOS6569* GetVic();
		CMOS6526A* GetCia1();

		int LoadApp(char* fname);
		int LoadBasic(char* fname);
		//int SavePRG
		
		void SetHiresTimeProvider(CHiresTime* hTime);
	
};

#endif
