/*
 *  CBM64Main.cpp
 *  A64Mac
 *
 *  Created by Bruno Keymolen on 12/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "CBM64Main.h"
#include "UTestOpcode.h"

void* CBM64ThreadProc(void* lpParameter);


int CBM64Main::Init(){
	std::cout << "---------------------------------\n";
    std::cout << "Commodore64 Emulator\n";
    std::cout << "Copyright (C)2008, Bruno Keymolen\n";
    std::cout << "---------------------------------\n";
	
	BKE_MUTEX_CREATE(mMutex);	 
	
	mBus = CBus::GetInstance();
	mVic = new CMOS6569();	
	mRam = new CRam();	
	mBasicRom = new CBasicRom();
	mKernalRom = new CKernalRom();
	mProcessor = new CMOS6510(mMutex);
	mCia1 = new CMOS6526A(mMutex);
	mCharRom = new CCharRom();
	
	
    return 0;
}


int CBM64Main::Run(){
	BKE_THREAD_CREATE(mCBM64Thread, &CBM64ThreadProc, this);
	return 0;
}


int CBM64Main::RunPriv(){
	
	#if 1 //Disable this for Unit Testing
		mProcessor->Run();
	#else
		//Unit tests
		CUTestOpcode* testOpcode = new CUTestOpcode(mProcessor, mBus);
		testOpcode->StartTest();
	#endif

	return 0;
}
	
	
int CBM64Main::Stop(){	
	delete mProcessor;
	delete mVic;
	delete mRam;
	delete mKernalRom;
	delete mBasicRom;
	delete mCia1;
	delete mCharRom;
	return 0;
}

CMOS6569* CBM64Main::GetVic(){
	return mVic;
}

CMOS6526A* CBM64Main::GetCia1(){
	return mCia1;
}

int CBM64Main::SetDisassemble(int d){
	mProcessor->mDisassemble = d;
	return mProcessor->mDisassemble;
}


int CBM64Main::GetDisassemble(){
	return mProcessor->mDisassemble;
}


int CBM64Main::LoadApp(char* fname){
	return mRam->LoadApp(fname);
}


int CBM64Main::LoadBasic(char* fname){
	return mRam->LoadBasic(fname);
}


void CBM64Main::SetHiresTimeProvider(CHiresTime* hTime){
	mProcessor->SetHiresTime(hTime);
}


void* CBM64ThreadProc(void* lpParameter){
	CBM64Main* pThis = (CBM64Main*)lpParameter;
	pThis->RunPriv();
	return NULL;
}




