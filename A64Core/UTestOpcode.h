/*
 *  UTestOpcode.h
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 16/06/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "Common.h"
#include "Memory.h"
#include "MOS6510.h"

#ifndef UTESTOPCODE_H
#define UTESTOPCODE_H


class CUTestOpcode{
public:
	u16 mBaseAddress;
private:
	CMOS6510* mProcessor;
	CBus* mMemory;
	//Tests
	int Test_ADC(u8 opcode, u8 A, u8 M, u8 C, char* info);
	int Test_JMP(u8 opcode, u8 toHB, u8 toLB, char* info);
	int CUTestOpcode::Test_SBC(u8 opcode, u8 A, u8 M, u8 C, u8 resA, u8 resP, char* info);

protected:
public:
	CUTestOpcode(CMOS6510* processor, CBus* bus);
	~CUTestOpcode();
	int StartTest();
};


#endif //UTESTOPCODE_H
