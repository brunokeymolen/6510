/*
 *  MOS6510Debug.h
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 08/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MOS6510DEBUG
#define MOS6510DEBUG


#include <fstream>
#include "Common.h"


#define EDLOG "../../../Data/edfull.log"

class CMOS6510Debug{
private:
    std::fstream* mStream;
	char* mBuf;
	int mBufLen;
protected:
public:
	CMOS6510Debug();
	~CMOS6510Debug();

	int GetTraceLine(u16* pc, u8* oppcode, u8* a, u8* x, u8* y, u8* sp, u8* p);
	void SkipTraceLine();

};


#endif //MOS6510DEBUG
