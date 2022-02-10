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
#include "MOS6526_CIA1.h"
#include "MOS6526_CIA2.h"
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
		CMOS6526CIA1* mCia1;
		CMOS6526CIA2* mCia2;
	    uint64_t mTotalCycles;

		BKE_THREAD mCBM64Thread;
		BKE_MUTEX mMutex;
	protected:
	public:
		int Init();
        int Cycle();
		int Stop();
        
        u8 Peek(u16 address) {
            if (mBus) {
                return mBus->Peek(address);
            }
            return 0xEE;
        }

		int SetDisassemble(int d);
		int GetDisassemble();
		
		CMOS6569* GetVic();
		CMOS6526CIA1* GetCia1();
		CMOS6526CIA2* GetCia2();

		int LoadApp(char* fname);
		int LoadBasic(char* fname);
		
		void SetHiresTimeProvider(CHiresTime* hTime);

        uint64_t GetCycles();
        
};

#endif
