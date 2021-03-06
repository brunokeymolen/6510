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
#include "MOS6510.h"
#include "MOS6569.h"
#include "Bus.h"
#include "Ram.h"
#include "BasicRom.h"
#include "KernalRom.h"
#include "MOS6526A.h"
#include "CharRom.h"
#include "General.h"
#include <thread>

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
        int Cycle();
		int Stop();

		int SetDisassemble(int d);
		int GetDisassemble();
		
		CMOS6569* GetVic();
		CMOS6526A* GetCia1();

		int LoadApp(char* fname);
		int LoadBasic(char* fname);
		
		void SetHiresTimeProvider(CHiresTime* hTime);

        uint64_t GetCycles();
        
};

#endif
