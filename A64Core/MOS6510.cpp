/*
 *  MOS6510.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 05/06/08.
 *  Copyright 2008 - 2010 Arcanegra BVBA. All rights reserved.
 * 
 *  @TODO: Add Decimal Mode
 */

#include "MOS6510.h"
#include "Util.h"
//#include <stdio.h>

//#define DEBUG_TRACE_OPCODE
//#define DEBUG_CONSISTENCY_CHECK
//#define DEBUG_DISABLE_IRQ
//#define DEBUG_SLOW_IRQ 60

static MOS6502Opcodes opcodeMatrix[] = 
{
    /*		//	0	1		2		3		4		5		6		7		8		9		10		11		12									*/
	/* ID	IMMED,	ZERO,	ZERO_X,	ZERO_Y,	IMPL,	ACCU,	ABSO,	ABSO_X,	ABSO_Y,	INDI,	INDI_X,	INDI_Y,	IND_REL,	ASSEMB,	Description		*/
	{ ADC,	0x69,	0x65,	0x75,	_x__,	_x__,	_x__,	0x6D,	0x7D,	0x79,	_x__,	0x61,	0x71,	_x__,		"ADC",  "Add memory to accumulator with carry"}	,
	{ AND,	0x29,	0x25,	0x35,	_x__,	_x__,	_x__,	0x2D,	0x3D,	0x39,	_x__,	0x21,	0x31,	_x__,		"AND",	"AND memory with accumulator" },	
	{ ASL,	_x__,	0x06,	0x16,	_x__,	_x__,	0x0A,	0x0E,	0x1E,	_x__,	_x__,	_x__,	_x__,	_x__,		"ASL",	"Shift left one bit (Memory or Accumulator)"},
	{ BCC,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0x90,		"BCC",	"Branch on carry clear" },
	{ BCS,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0xB0,		"BCS",	"Branch on carry set" },
	{ BEQ,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0xF0,		"BEQ",	"Branch on result zero" },
	{ BIT,	_x__,	0x24,	_x__,	_x__,	_x__,	_x__,	0x2C,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"BIT",	"Test bits in memory with accumulator" },
	{ BMI,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0x30,		"BMI",	"Branch on result minus" },
	{ BNE,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0xD0,		"BNE",	"Branch on result not zero" },
	{ BPL,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0x10,		"BPL",	"Branch on result plus" },
	{ BRK,	_x__,	_x__,	_x__,	_x__,	0x00,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"BRK",	"Force Break" },
	{ BVC,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0x50,		"BVC",	"Branch on overflow clear" },
	{ BVS,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0x70,		"BVC",	"Branch on overflow set" },
	{ CLC,	_x__,	_x__,	_x__,	_x__,	0x18,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"CLC",	"Clear carry flag" },
	{ CLD,	_x__,	_x__,	_x__,	_x__,	0xD8,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"CLD",	"Clear decimal mode" },
	{ CLI,	_x__,	_x__,	_x__,	_x__,	0x58,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"CLI",	"Clear interrup disable bit" },
	{ CLV,	_x__,	_x__,	_x__,	_x__,	0xB8,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"CLV",	"Clear overflow flag" },
	{ CMP,	0xC9,	0xC5,	0xD5,	_x__,	_x__,	_x__,	0xCD,	0xDD,	0xD9,	_x__,	0xC1,	0xD1,	_x__,		"CMP",	"Compare memory and accumulator" },
	{ CPX,	0xE0,	0xE4,	_x__,	_x__,	_x__,	_x__,	0xEC,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"CPX",	"Compare memory and index X" },
	{ CPY,	0xC0,	0xC4,	_x__,	_x__,	_x__,	_x__,	0xCC,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"CPY",	"Compare memory and index Y" },
	{ DEC,	_x__,	0xC6,	0xD6,	_x__,	_x__,	_x__,	0xCE,	0xDE,	_x__,	_x__,	_x__,	_x__,	_x__,		"DEC",	"Decrement Memory" },
	{ DEX,	_x__,	_x__,	_x__,	_x__,	0xCA,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"DEX",	"Decrement X Register " },
	{ DEY,	_x__,	_x__,	_x__,	_x__,	0x88,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"DEY",	"Decrement Y Register " },
	{ EOR,	0x49,	0x45,	0x55,	_x__,	_x__,	_x__,	0x4D,	0x5D,	0x59,	_x__,	0x41,	0x51,	_x__,		"EOR",	"Exclusive OR " },
	{ INC,	_x__,	0xE6,	0xF6,	_x__,	_x__,	_x__,	0xEE,	0xFE,	_x__,	_x__,	_x__,	_x__,	_x__,		"INC",	"Increment Memory" },
	{ INX,	_x__,	_x__,	_x__,	_x__,	0xE8,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"INX",	"Increment X Register" },
	{ INY,	_x__,	_x__,	_x__,	_x__,	0xC8,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"INY",	"Increment Y Register" },
	{ JMP,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0x4C,	_x__,	_x__,	0x6C,	_x__,	_x__,	_x__,		"JMP",	"Jump" },
	{ JSR,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	0x20,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"JSR",	"Jump to Subroutine" },
	{ LDA,	0xA9,	0xA5,	0xB5,	_x__,	_x__,	_x__,	0xAD,	0xBD,	0xB9,	_x__,	0xA1,	0xB1,	_x__,		"LDA",	"Load Accumulator" },
	{ LDX,	0xA2,	0xA6,	_x__,	0xB6,	_x__,	_x__,	0xAE,	_x__,	0xBE,	_x__,	_x__,	_x__,	_x__,		"LDX",	"Load X Register" },
	{ LDY,	0xA0,	0xA4,	0xB4,	_x__,	_x__,	_x__,	0xAC,	0xBC,	_x__,	_x__,	_x__,	_x__,	_x__,		"LDY",	"Load Y Register" },
	{ LSR,	_x__,	0x46,	0x56,	_x__,	_x__,	0x4A,	0x4E,	0x5E,	_x__,	_x__,	_x__,	_x__,	_x__,		"LSR",	"Logical Shift Right" },
	{ NOP,	_x__,	_x__,	_x__,	_x__,	0xEA,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"NOP",	"No Operation" },
	{ ORA,	0x09,	0x05,	0x15,	_x__,	_x__,	_x__,	0x0D,	0x1D,	0x19,	_x__,	_x__,	_x__,	_x__,		"ORA",	"Logical Inclusive OR" },
	{ PHA,	_x__,	_x__,	_x__,	_x__,	0x48,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"PHA",	"Push Accumulator" },
	{ PHP,	_x__,	_x__,	_x__,	_x__,	0x08,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"PHP",	"Push Processor Status" },
	{ PLA,	_x__,	_x__,	_x__,	_x__,	0x68,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"PLA",	"Pull Accumulator" },
	{ PLP,	_x__,	_x__,	_x__,	_x__,	0x28,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"PLP",	"Pull Processor Status" },
	{ ROL,	_x__,	0x26,	0x36,	_x__,	_x__,	0x2A,	0x2E,	0x3E,	_x__,	_x__,	_x__,	_x__,	_x__,		"ROL",	"Rotate Left" },
	{ ROR,	_x__,	0x66,	0x76,	_x__,	_x__,	0x6A,	0x6E,	0x7E,	_x__,	_x__,	_x__,	_x__,	_x__,		"ROR",	"Rotate Right" },
	{ RTI,	_x__,	_x__,	_x__,	_x__,	0x40,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"RTI",	"Return from Interrupt" },
	{ RTS,	_x__,	_x__,	_x__,	_x__,	0x60,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"RTS",	"Return from Subroutine" },
	{ SBC,	0xE9,	0xE5,	0xF5,	_x__,	_x__,	_x__,	0xED,	0xFD,	0xF9,	_x__,	0xE1,	0xF1,	_x__,		"SBC",	"Subtract with Carry " },
	{ SEC,	_x__,	_x__,	_x__,	_x__,	0x38,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"SEC",	"Set Carry Flag" },
	{ SED,	_x__,	_x__,	_x__,	_x__,	0xF8,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"SED",	"Set Decimal Flag" },
	{ SEI,	_x__,	_x__,	_x__,	_x__,	0x78,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"SEI",	"Set Interrupt Disable" },
	{ STA,	_x__,	0x85,	0x95,	_x__,	_x__,	_x__,	0x8D,	0x9D,	0x99,	_x__,	0x81,	0x91,	_x__,		"STA",	"Store Accumulator" },
	{ STX,	_x__,	0x86,	_x__,	0x96,	_x__,	_x__,	0x8E,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"STX",	"Store X Register" },
	{ STY,	_x__,	0x84,	0x94,	_x__,	_x__,	_x__,	0x8C,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"STY",	"Store Y Register" },
	{ TAX,	_x__,	_x__,	_x__,	_x__,	0xAA,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"TAX",	"Transfer Accumulator to X" },
	{ TAY,	_x__,	_x__,	_x__,	_x__,	0xA8,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"TAY",	"Transfer Accumulator to Y" },
	{ TSX,	_x__,	_x__,	_x__,	_x__,	0xBA,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"TSX",	"Transfer Stack Pointer to X" },
	{ TXA,	_x__,	_x__,	_x__,	_x__,	0x8A,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"TXA",	"Transfer X to Accumulator" },
	{ TXS,	_x__,	_x__,	_x__,	_x__,	0x9A,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"TXS",	"Transfer X to Stack Pointer" },
	{ TYA,	_x__,	_x__,	_x__,	_x__,	0x98,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"TYA",	"Transfer Y to Accumulator" },

	{ END_OPC,		_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"XXX",	"End Matrix" },
	{ ILLEGAL_OPC,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,	_x__,		"___",	"Illigal opcode" },
	/* ID	IMMED,	ZERO,	ZERO_X,	ZERO_Y,	IMPL,	ACCU,	ABSO,	ABSO_X,	ABSO_Y,	INDI,	INDI_X,	INDI_Y,	IND_REL,	ASSEMB,	Description		*/
};


//CMOS6510::CMOS6510(CMemory* mem){
CMOS6510::CMOS6510(BKE_MUTEX mutex){

	mMutex = mutex;
	
	mMemory = mBus = CBus::GetInstance();
	
	//Init the registers
	r_pc	= mMemory->Peek16(0xFFFC);
	r_sp	= 0x00; 
	r_a		= 0x00;
	r_x		= 0x00;
	r_y		= 0x00;
	r_p		= 0x20;
	
	

	
	//Reset the Processor Flags (p) register
	RESETFLAGS; 
	
	
	int listRow, matrixRow, addressMode;
	//Create linear list of opcodes and functions
	for(listRow=0;listRow<OPCODESMAX;listRow++){
		mOpcodes[listRow].matrixID = ILLEGAL_OPC;
		mOpcodes[listRow].assembly = "___";
		matrixRow=0;
		while(true){
			if(opcodeMatrix[matrixRow].ID == END_OPC){
				break;
			}
			for(addressMode=0;addressMode<ADDRESS_MODE_MAX;addressMode++){
				if( (listRow == opcodeMatrix[matrixRow].oppcodes[addressMode]) && 
					(opcodeMatrix[matrixRow].oppcodes[addressMode] != _x__)){
					//Store in list
					mOpcodes[listRow].matrixID = opcodeMatrix[matrixRow].ID;
					mOpcodes[listRow].addressMode = (u8)addressMode;
					mOpcodes[listRow].assembly = opcodeMatrix[matrixRow].assembly;
					mOpcodes[listRow].info = opcodeMatrix[matrixRow].description;
					switch(mOpcodes[listRow].matrixID){
						case ADC:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_ADC;
							break;
						case AND:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_AND;
							break;
						case ASL:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_ASL;
							break;
						case BCC:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BCC;
							break;
						case BCS:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BCS;
							break;
						case BEQ:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BEQ;
							break;
						case BIT:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BIT;
							break;
						case BMI:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BMI;
							break;
						case BNE:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BNE;
							break;
						case BPL:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BPL;
							break;
						case BRK:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BRK;
							break;
						case BVC:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BVC;
							break;
						case BVS:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_BVS;
							break;
						case CLC:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_CLC;
							break;
						case CLD:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_CLD;
							break;
						case CLI:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_CLI;
							break;
						case CLV:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_CLV;
							break;
						case CMP:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_CMP;
							break;
						case CPX:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_CPX;
							break;
						case CPY:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_CPY;
							break;
						case DEC:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_DEC;
							break;
						case DEX:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_DEX;
							break;
						case DEY:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_DEY;
							break;
						case EOR:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_EOR;
							break;
						case INC:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_INC;
							break;
						case INX:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_INX;
							break;
						case INY:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_INY;
							break;
						case JMP:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_JMP;
							break;
						case JSR:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_JSR;
							break;
						case LDA:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_LDA;
							break;
						case LDX:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_LDX;
							break;
						case LDY:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_LDY;
							break;
						case LSR:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_LSR;
							break;
						case NOP:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_NOP;
							break;
						case ORA:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_ORA;
							break;
						case PHA:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_PHA;
							break;
						case PHP:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_PHP;
							break;
						case PLA:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_PLA;
							break;
						case PLP:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_PLP;
							break;
						case ROL:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_ROL;
							break;
						case ROR:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_ROR;
							break;
						case RTI:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_RTI;
							break;
						case RTS:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_RTS;
							break;
						case SBC:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_SBC;
							break;
						case SEC:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_SEC;
							break;
						case SED:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_SED;
							break;
						case SEI:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_SEI;
							break;
						case STA:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_STA;
							break;
						case STX:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_STX;
							break;
						case STY:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_STY;
							break;
						case TAX:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_TAX;
							break;
						case TAY:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_TAY;
							break;
						case TSX:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_TSX;
							break;
						case TXA:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_TXA;
							break;
						case TXS:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_TXS;
							break;
						case TYA:
							mOpcodes[listRow].opcodeFunction = &CMOS6510::F_TYA;
							break;
						default:
							mOpcodes[listRow].opcodeFunction = NULL;
							break;
					}  
				}
			}
			matrixRow++;
		}
	}
	
	//Dump list
//	for(int m=0;m<256;m++){
//		printf("0x%.2x - %.3s ; %.2d\n", m, mOpcodes[m].assembly, mOpcodes[m].addressMode);
//	}	
	
	mDisassemble = 0;
	
	mOpcodeDebug = new CMOS6510Debug();

	
}



CMOS6510::~CMOS6510(){
	delete mOpcodeDebug;

}


void CMOS6510::SetHiresTime(CHiresTime* hiresTime){
	mHiresTime = hiresTime;
}


/*
 * Stack operations
 */
void CMOS6510::Push(u8 val){
	mMemory->Poke(STACKOFFSET + r_sp, val);
	r_sp--;
//	CUtil::HexDumpMemory(mMemory,STACKOFFSET+255-16,32);
}


u8 CMOS6510::Pop(){
	r_sp++;
//	CUtil::HexDumpMemory(mMemory,STACKOFFSET+255-16,32);
	return mMemory->Peek(STACKOFFSET + r_sp);
}

void CMOS6510::Push16(u16 val){
	u8 b;
	//High
	b = (u8)((val & 0xFF00) >> 8);
	Push(b);
	//Low
	b = (u8)(val & 0x00FF);
	Push(b);
}


u16 CMOS6510::Pop16(){
	u16 b = 0;
	u16 h;
	//Low
	b = Pop();
	//High
	h = Pop() << 8;
	//Add
	b = b | h;
	return b;
}


/*
 * Run Loop
 */
void CMOS6510::Run(){
	u8 cmd;
	u16 prevPC;
	timeval t;//, start, end, irqstart, now, previr;
	long mips, mipsactive;
	bool ir;
	int prevIrTime, timeNow, startMips;

	
	#ifdef DEBUG_TRACE_OPCODE 
		cout << hex << setfill('0') << uppercase; 
		int trace = 1;
		#ifdef DEBUG_CONSISTENCY_CHECK
			mOpCnt = 0;
			trace=1;
		#endif	
	#endif

//mDisassemble = 0;
//trace = 0;	
	
	ir = false;
//	gettimeofday(&start, NULL);	
	gettimeofday(&t, NULL);	
	

	mips = 0;
	startMips = prevIrTime = ((t.tv_sec * 1000000) + t.tv_usec) ;//mHiresTime->GetMicrosecondsLo();
	
	
	while(1){
//		BKE_MUTEX_LOCK(mMutex);
	

		
		#ifdef DEBUG_CONSISTENCY_CHECK
		mOpCnt = mOpCnt + 1;
		#endif

		/* DEBUG
		if(mOpCnt == 0x996FA){
			u8 bufPos = mBus->Peek(0xC6);
			mBus->PokeDevice(eBusRam, 0x0277+bufPos, '?');
			mBus->Poke(0xC6, bufPos+1);

			bufPos = mBus->Peek(0xC6);			
			mBus->PokeDevice(eBusRam, 0x0277+bufPos, 'S');
			mBus->Poke(0xC6, bufPos+1);
			
			bufPos = mBus->Peek(0xC6);			
			mBus->PokeDevice(eBusRam, 0x0277+bufPos, 'I');
			mBus->Poke(0xC6, bufPos+1);

			bufPos = mBus->Peek(0xC6);			
			mBus->PokeDevice(eBusRam, 0x0277+bufPos, 'N');
			mBus->Poke(0xC6, bufPos+1);

			bufPos = mBus->Peek(0xC6);			
			mBus->PokeDevice(eBusRam, 0x0277+bufPos, '(');
			mBus->Poke(0xC6, bufPos+1);

			bufPos = mBus->Peek(0xC6);			
			mBus->PokeDevice(eBusRam, 0x0277+bufPos, '6');
			mBus->Poke(0xC6, bufPos+1);

			bufPos = mBus->Peek(0xC6);			
			mBus->PokeDevice(eBusRam, 0x0277+bufPos, '3');
			mBus->Poke(0xC6, bufPos+1);

			bufPos = mBus->Peek(0xC6);			
			mBus->PokeDevice(eBusRam, 0x0277+bufPos, ')');
			mBus->Poke(0xC6, bufPos+1);

			bufPos = mBus->Peek(0xC6);			
			mBus->PokeDevice(eBusRam, 0x0277+bufPos, 0x0D);
			mBus->Poke(0xC6, bufPos+1);

		}
	*/
	
		cmd = mMemory->Peek(r_pc);
		prevPC = r_pc;
		
//		if(r_pc==0xFD9A){
//			trace = 1;
//			cout << "done" << endl;
//		}
//		if(r_pc==0xFF73){
//			cout << "done" << endl;
//		}
		
		
		
		if(mOpcodes[cmd].matrixID != ILLEGAL_OPC && mOpcodes[cmd].opcodeFunction != NULL){
			r_pc++; //increment (see: http://www.6502.org/tutorials/6502opcodes.html#PC)
			(*this.*(mOpcodes[cmd].opcodeFunction))( (Mos6502AddressMode)mOpcodes[cmd].addressMode ); //C++ Function pointers have a weird syntax...
		}else{
			cout << "Illegal Opcode Error. Exit Emulator." << endl;
			DBGTraceLine(cmd, prevPC);
			cout << "Opcode = 0x" << hex << setfill('0') << setw(2)  << (int)cmd << ", address = 0x" << setw(4) << r_pc << endl;
			CUtil::HexDumpMemory(mMemory, r_pc , 64);
//			BKE_MUTEX_UNLOCK(mMutex);
			return;
		}
		
#ifdef DEBUG_TRACE_OPCODE
		if(trace == 1 || mDisassemble > 0){
			DBGTraceLine(cmd, prevPC);
		}else{
			mOpcodeDebug->SkipTraceLine();
		}

		if(mDisassemble > 1){
			usleep(mDisassemble*1000);
		}
#endif		
//		BKE_MUTEX_UNLOCK(mMutex);
		
//cout << "mHiresTime->GetMicroseconds(" << mHiresTime->GetMicroseconds() << ") - prevIrTime:" << prevIrTime << endl;
		mips++;
		mipsactive++;
#ifndef DEBUG_DISABLE_IRQ
		
		if(mipsactive >= 25000){		
			//IRQ
			gettimeofday(&t, NULL);	
			timeNow = ((t.tv_sec * 1000000) + t.tv_usec); //mHiresTime->GetMicrosecondsLo();
			if(ir){
#ifdef DEBUG_SLOW_IRQ			
				if(timeNow - prevIrTime >= 20000 * DEBUG_SLOW_IRQ){
#else
				if(timeNow - prevIrTime >= 20000){
#endif				
					if(ISFLAG(FLAG_I) == 0){
						//Maskable IRQ
						IRQ();
					}
					//Non Maskable IRQ
//					NMI();
					prevIrTime = timeNow;
				}
			}else{
#ifdef DEBUG_SLOW_IRQ			
				if(timeNow - prevIrTime >= 500000 * DEBUG_SLOW_IRQ){
#else				
				if(timeNow - prevIrTime >= 500000){
#endif				
					ir = true; // Enable IR
		//			trace = 1;
					prevIrTime = timeNow;
				}
			}//ir
				
			//MIPS
			if(timeNow - startMips >= 5000000){
				cout << "mips (*1000) = " << dec << (mips / 1000) / 5 << endl;
				mips = 0;
				startMips = timeNow;
			}
	
		}
		
#endif // DEBUG_DISABLE_IRQ

        
	}
}


void CMOS6510::DBGTraceLine(u8 cmd, u16 prevPC){
		
		#ifdef DEBUG_CONSISTENCY_CHECK	
			static u16 dbgPc;
			static u8 dbgOp, dbgA, dbgX, dbgY, dbgSp, dbgP;
			cout << setfill('0') << setw(4) << hex << mOpCnt << "; ";
 
			mOpcodeDebug->GetTraceLine(&dbgPc, &dbgOp, &dbgA, &dbgX, &dbgY, &dbgSp, &dbgP);
		#endif
		
		#ifdef DEBUG_TRACE_OPCODE
		
			cout << setfill('0') << setw(4) << hex << prevPC ;
			cout << " " << mOpcodes[cmd].assembly << " ";
			if(mOpcodes[cmd].matrixID != ILLEGAL_OPC){
				PrintOperands(mOpcodes[cmd].addressMode, prevPC);
			}
			cout << " - 0x" << setw(2) << setfill('0') << hex << (int)cmd;
			cout << ", Next PC=0x" << setw(2) << setfill('0') << hex << (int)r_pc;
			cout << ", SP=0x" << setw(2) << setfill('0') << hex << (int)r_sp;
			cout << ": A=0x" << setw(2) << hex << (int)r_a << ", X=0x" << setw(2) << (int)r_x << ", Y=0x" << setw(2)<< (int)r_y; 
			cout << ", P=";
			PrintStatusBits();
			cout << " (" << mOpcodes[cmd].info << ")";
			cout << endl;
		#endif //DEBUG_TRACE_OPCODE
	
		#ifdef DEBUG_CONSISTENCY_CHECK	
		if(prevPC != dbgPc || cmd != dbgOp || r_a != dbgA || r_x != dbgX || r_y != dbgY || r_p != dbgP){
			cout << "inconsistent!!!!!" << endl;
			exit(-1);
		}
		#endif

}



/*
 * Returns the Address where the opcode has to find the information to execute 
 * (mostly where the operands can be found)
 * 
 * imm = #$00 
 * zp = $00 
 * zpx = $00,X 
 * zpy = $00,Y 
 * izx = ($00,X) 
 * izy = ($00),Y 
 * abs = $0000 
 * abx = $0000,X 
 * aby = $0000,Y 
 * ind = ($0000) 
 * rel = $0000 (PC-relative) 
 */
bool CMOS6510::GetOperandAddress(u8 addressMode, u16* address){ 
	bool ret = true;
	switch(addressMode){
		ADDRESS_MODE_IMPLIED:	
		ADDRESS_MODE_ACCUMULATOR:
			*address = 0; //Should not happen
			cout << "Warning: ADDRESS_MODE_IMPLIED | ADDRESS_MODE_ACCUMULATOR, GetOperandAddress should not be called." << endl;
		case ADDRESS_MODE_IMMEDIATE:   //imm = #$00 ; (Immediate)
			*address = r_pc;
			r_pc = r_pc + 1;
			break;
		case ADDRESS_MODE_ZEROPAGE:   //zp = $00 ; (Zero Page Absolute)
			*address = (u16)(mMemory->Peek(r_pc) & 0xFF);
			r_pc = r_pc + 1;
			break;
		case ADDRESS_MODE_ZEROPAGE_X: //zpx = $00,X ; (Zero Page Index)
			*address = ((u16)(mMemory->Peek(r_pc)+r_x)) &0xFF;
			r_pc = r_pc + 1;
			break;
		case ADDRESS_MODE_ZEROPAGE_Y: //zpy = $00,Y ; (Zero Page Index)
			*address = ((u16)(mMemory->Peek(r_pc)+r_y)) &0xFF;
			r_pc = r_pc + 1;
			break;
		case ADDRESS_MODE_INDIRECT_X:  //izx = ($00,X) ; ( Pre Index Indirect)
			*address = (u16)( ((u8)(mMemory->Peek(r_pc)+r_x)) & 0xFF); 
			*address = mMemory->Peek16(*address); 
			r_pc = r_pc + 1 ;
			break;
		case ADDRESS_MODE_INDIRECT_Y:  //izy = ($00),Y ; (Post Index Indirect)
			*address = (u16)( mMemory->Peek(r_pc) & 0xFF ); 
			*address = mMemory->Peek16(*address) + r_y; 
			r_pc = r_pc + 1 ; 
			break;
		case ADDRESS_MODE_ABSOLUTE:  //abs = $0000 ; (Absolute)
			*address = mMemory->Peek16(r_pc);
			r_pc = r_pc + 2 ;
			break;			
		case ADDRESS_MODE_ABSOLUTE_X: //abx = $0000,X ; (Index)
			*address = mMemory->Peek16(r_pc) + r_x;
			r_pc = r_pc + 2;
			break;
		case ADDRESS_MODE_ABSOLUTE_Y: //aby = $0000,Y ; (Index)
			*address = mMemory->Peek16(r_pc) + r_y;
			r_pc = r_pc + 2;
			break;
		case ADDRESS_MODE_INDIRECT:// ind = ($0000) ; (Indirect) 
			*address = mMemory->Peek16(r_pc); 
			*address = mMemory->Peek16(*address);
			r_pc = r_pc + 2 ; 
			break;
		case ADDRESS_MODE_RELATIVE://rel = $0000 (PC-relative)
			*address = r_pc;
			r_pc = r_pc + 1;
			break;	
		default:
			cout << "Unknown address mode error : " << dec << addressMode << endl;
			ret = false;
			break;
	}
	
	//cout << "(0x" << hex << setw(4) << (int)*address << ": 0x" << setw(2) << (int)address[0] << ", 0x" << (int)address[1] << ", 0x" << (int)address[2] << ")";

	
	return ret;
}


bool CMOS6510::DBGRunOneInstruction(u16* pc, u16* sp, u8* a, u8* x, u8* y, u8* p){
	r_pc = *pc;
	r_sp = *sp;
	r_a = *a;
	r_x = *x;
	r_y = *y;
	r_p = *p;
	
	u8 cmd = mMemory->Peek(r_pc);
//	cout << "r_pc = 0x" << setfill('0') << setw(4) << hex << r_pc << ", 0x" << setw(2) << (int)cmd << endl;
		
	if(mOpcodes[cmd].matrixID != ILLEGAL_OPC && mOpcodes[cmd].opcodeFunction != NULL){
		r_pc++; //increment (see: http://www.6502.org/tutorials/6502opcodes.html#PC)
		(*this.*(mOpcodes[cmd].opcodeFunction))( mOpcodes[cmd].addressMode ); //C++ Function pointers have a weird syntax...
	}else{
		cout << "Degug; Illegal Opcode Error." << endl;
		cout << "Opcode = 0x" << hex << setfill('0') << setw(2)  << (int)cmd << ", address = 0x" << setw(4) << r_pc << endl;
		return false;
	}

	*a = r_a;
	*x = r_x;
	*y = r_y;
	*p = r_p;
	*pc = r_pc;
	*sp = r_sp;
		
	return true;
}


/*
 * ADC - Add memory to accumulator with carry
 * @TODO: Add decimal mode
 */
void CMOS6510::F_ADC(u8 addressmode){
	u16 address;
	u8 oper; 
	u16 val; //take a u16 to have place for the carry bit
	
	GetOperandAddress(addressmode, &address);
	oper = mMemory->Peek(address);
	
	val = r_a + oper + (u8)(ISFLAG(FLAG_C));
		
	//copy the low 8 bits to r_a
	r_a = (u8)(val); // & 0x00FF);  
	
	CLRFLAG(FLAG_C);	
	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_V);
	CLRFLAG(FLAG_N);

//	if(val > 0xFF)SETFLAG(FLAG_C);
	if((val & 0x100) > 0)SETFLAG(FLAG_C);
	if(r_a == 0)SETFLAG(FLAG_Z);
	if( (r_a & 0x80) > 0 ) SETFLAG(FLAG_N);
	if( (s16)val > 127 || (s16)val < -128 ) SETFLAG(FLAG_V);
}

/*
 * AND - Logical And
 */
void CMOS6510::F_AND(u8 addressmode){ //Mos6502AddressMode
	u16 address;
	u8 val;
	
	GetOperandAddress(addressmode, &address);
	val = mMemory->Peek(address);
		
	r_a = r_a & val;

	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);

	if(r_a == 0)SETFLAG(FLAG_Z);
	if( (r_a & 0x80) > 0 ) SETFLAG(FLAG_N);
}

/*
 * ASL - Arithmetic Shift Left
 */
void CMOS6510::F_ASL(u8 addressmode){
	u16 address;
	u8 val;
	
	
	//Are we acting on memory or accummulator ?
	if(addressmode == ADDRESS_MODE_ACCUMULATOR){
		 val = r_a;		
	}else{	
		GetOperandAddress(addressmode, &address);
		val = mMemory->Peek(address);
	}
	
	if((val & 0x80) > 0){
		SETFLAG(FLAG_C);
	}else{
		CLRFLAG(FLAG_C);
	} 
	
	val = val << 1;
			
	if(addressmode == ADDRESS_MODE_ACCUMULATOR){
		r_a = val;		
	}else{	
		mMemory->Poke(address, val);
	}

	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);
	
	if(val == 0)SETFLAG(FLAG_Z);
	if( (val & 0x80) > 0 ) SETFLAG(FLAG_N);
}

/*
 * BCC - Branch if Carry Clear 
 */
void CMOS6510::F_BCC(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);
	
	if(ISFLAG(FLAG_C) == 0){
		r_pc = r_pc + (s8)mMemory->Peek(address); 
	}
}

/*
 * BCS - Branch if Carry Set 
 */
void CMOS6510::F_BCS(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);
	
	if(ISFLAG(FLAG_C) == 1){
		r_pc = r_pc + (s8)mMemory->Peek(address); 
	}
}

/*
 * BEQ - Branch if Equal 
 */
void CMOS6510::F_BEQ(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);
	
	if(ISFLAG(FLAG_Z) == 1){
		r_pc = r_pc + (s8)mMemory->Peek(address); 
	}
}

/*
 * BIT - Bit Test 
 */
void CMOS6510::F_BIT(u8 addressmode){
	u16 address;
	u8 val;
	u8 m;
	
	GetOperandAddress(addressmode, &address);
	
	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_V);
	CLRFLAG(FLAG_N);

	m = mMemory->Peek(address); 

	val = (m & r_a);
	
	if(val == 0){
		SETFLAG(FLAG_Z);
	}
	if((m & FLAG_V) > 0){
		SETFLAG(FLAG_V);
	}
	if((m & FLAG_N) > 0){
		SETFLAG(FLAG_N);
	}
}

/* 
 * BMI - Branch if Minus
 */
void CMOS6510::F_BMI(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);
	
	if(ISFLAG(FLAG_N) == 1){
		r_pc = r_pc + (s8)mMemory->Peek(address);
	}
}
 
/*
 * BNE - Branch Not Equal  
 */
void CMOS6510::F_BNE(u8 addressmode){
	u16 address;
	GetOperandAddress(addressmode, &address);
	
	if(ISFLAG(FLAG_Z) == 0){
		r_pc = r_pc + (s8)mMemory->Peek(address);  
	}
	
}

/* 
 * BMI - Branch if Positive
 */
void CMOS6510::F_BPL(u8 addressmode){
	u16 address;
	GetOperandAddress(addressmode, &address);
	
	if(ISFLAG(FLAG_N) == 0){
		r_pc = r_pc + (s8)mMemory->Peek(address); 
	}
}
 
/* 
 * BRK - Force an Interrupt or Break
 * @TODO: Check
 */
void CMOS6510::F_BRK(u8 addressmode){
cout << "BRK" << endl;
//	r_pc++;	
	SETFLAG(FLAG_B);
	Push16( r_pc );
	Push(r_p);
	SETFLAG(FLAG_I);
	r_pc = mMemory->Peek16(0xFFFE);
}
 

 
/* 
 * BVC - Branch if Overflow Clear
 */
void CMOS6510::F_BVC(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);

	if(ISFLAG(FLAG_V) == 0){
		r_pc = r_pc + (s8)mMemory->Peek(address); 
	}
}

/* 
 * BVS - Branch if Overflow Set 
 */
void CMOS6510::F_BVS(u8 addressmode){
	u16 address;
	GetOperandAddress(addressmode, &address);
	
	if(ISFLAG(FLAG_V) == 1){
		r_pc = r_pc + (s8)mMemory->Peek(address); 
	}
}

/*
 * CLC - Clear Carry Flag
 */
void CMOS6510::F_CLC(u8 addressmode){
	CLRFLAG(FLAG_C);
}

/*
 * CLD - Clear Decimal Mode
 */
void CMOS6510::F_CLD(u8 addressmode){
	CLRFLAG(FLAG_D);
}

/*
 * CLI - Clear Interrupt Disable 
 */
void CMOS6510::F_CLI(u8 addressmode){
	CLRFLAG(FLAG_I);
}

/*
 * CLV - Clear Overflow Flag
 */
void CMOS6510::F_CLV(u8 addressmode){
	CLRFLAG(FLAG_V);
}


/*
 * CMP - Compare
 */
void CMOS6510::F_CMP(u8 addressmode){
	u16 address;
	u8 m;
	s8 val;
	
	GetOperandAddress(addressmode, &address);	
	m = mMemory->Peek(address);
	
	CLRFLAG(FLAG_C);
	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);

	val = r_a - m;
	
	if(r_a == m){
		SETFLAG(FLAG_Z);
	}
	if( (val & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}
	if(m <= r_a){
		SETFLAG(FLAG_C);
	}
}


/*
 * CPX - Compare X Register
 */
void CMOS6510::F_CPX(u8 addressmode){
	u16 address;
	u8 m;
	s8 val;
	
	GetOperandAddress(addressmode, &address);	
	m = mMemory->Peek(address);
	
	CLRFLAG(FLAG_C);
	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);

	val = r_x - m;
	
	if(r_x == m){
		SETFLAG(FLAG_Z);
	}
	if( (val & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}
	if(m <= r_x){
		SETFLAG(FLAG_C);
	}
}


/*
 * CPY - Compare Y Register
 */
void CMOS6510::F_CPY(u8 addressmode){
	u16 address;
	u8 m;
	s8 val;
	
	GetOperandAddress(addressmode, &address);	
	m = mMemory->Peek(address);
	
	CLRFLAG(FLAG_C);
	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);

	val = r_y - m;
	
	if(r_y == m){
		SETFLAG(FLAG_Z);
	}
	if( (val & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}
	if(m <= r_y){
		SETFLAG(FLAG_C);
	}
}


/*
 * DEC - Decrement Memory
 */ 
void CMOS6510::F_DEC(u8 addressmode){
	u16 address;
	u8 m;
	
	GetOperandAddress(addressmode, &address);	
	m = mMemory->Peek(address);

	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);
	
		
	m = m - 1;
	
	if(m & 0x80){
		SETFLAG(FLAG_N);		
	}
	if(m == 0){
		SETFLAG(FLAG_Z);		
	}
	
	mMemory->Poke(address, m);
};


/*
 * DEX - Decrement X Register 
 */
void CMOS6510::F_DEX(u8 addressmode){
	//Only Implied Mode
	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);	
		
	r_x = r_x - 1;
	
	if(r_x == 0){
		SETFLAG(FLAG_Z);		
	}
	if( (r_x & 0x80) > 0){
		SETFLAG(FLAG_N);		
	}
}


/*
 * DEY - Decrement Y Register 
 */
void CMOS6510::F_DEY(u8 addressmode){
	//Only Implied Mode
	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);
	
		
	r_y = r_y - 1;
	
	if(r_y == 0){
		SETFLAG(FLAG_Z);		
	}
	if( (r_y & 0x80) > 0){
		SETFLAG(FLAG_N);		
	}
}

/*
 * EOR - Exclusive OR
 */
void CMOS6510::F_EOR(u8 addressmode){
	u16 address;
	u8 m;
	
	GetOperandAddress(addressmode, &address);	
	m = mMemory->Peek(address);

	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);
	
	r_a = r_a ^ m;
	
	if(r_a == 0){
		SETFLAG(FLAG_Z);				
	}
	
	if(r_a & 0x80){
		SETFLAG(FLAG_N);
	}			 			
}


/*
 * INC - Increment Memory 
 * M,Z,N = M+1 
 */
void CMOS6510::F_INC(u8 addressmode){
	u16 address;
	u8 m;
	
	GetOperandAddress(addressmode, &address);	
	m = mMemory->Peek(address);

	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);
	
		
	m = m + 1;
	
	if(m & 0x80){
		SETFLAG(FLAG_N);		
	}
	if(m == 0){
		SETFLAG(FLAG_Z);		
	}
	
	mMemory->Poke(address, m);
}

/*
 * INX - Increment X Register 
 * X,Z,N = X+1 
 */
void CMOS6510::F_INX(u8 addressmode){
	//Only Implied Mode
	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);	
		
	r_x = r_x + 1;
	
	if(r_x == 0){
		SETFLAG(FLAG_Z);		
	}
	if( (r_x & 0x80) > 0){
		SETFLAG(FLAG_N);		
	}
}


/*
 * INY - Increment Y Register 
 * Y,Z,N = Y+1 
 */
void CMOS6510::F_INY(u8 addressmode){
	//Only Implied Mode
	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);	
		
	r_y = r_y + 1;
	
	if(r_y == 0){
		SETFLAG(FLAG_Z);		
	}
	if( (r_y & 0x80) > 0){
		SETFLAG(FLAG_N);		
	}
}

/* 
 * JMP - Jump 
 */
void CMOS6510::F_JMP(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);	
	r_pc = address;
}


/* 
 * JSR - Jump to Subroutine 
 */
void CMOS6510::F_JSR(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);	

	Push16(r_pc-1);

	r_pc = address;

}

/*
 * LDA - Load Accumulator 
 * A,Z,N = M 
 */
void CMOS6510::F_LDA(u8 addressmode){
	u16 address;
	u8 m;
	
	GetOperandAddress(addressmode, &address);	
	m = mMemory->Peek(address);

	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);
		
	r_a = m;
	
	if(m & 0x80){
		SETFLAG(FLAG_N);		
	}
	if(m == 0){
		SETFLAG(FLAG_Z);		
	}
	
}


/*
 * LDX - Load X Register 
 * X,Z,N = M
 */
void CMOS6510::F_LDX(u8 addressmode){
	u16 address;
	u8 m;
	
	GetOperandAddress(addressmode, &address);	
	m = mMemory->Peek(address);

	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);
	
		
	r_x = m;
	
	if((m & 0x80)>0){
		SETFLAG(FLAG_N);		
	}
	if(m == 0){
		SETFLAG(FLAG_Z);		
	}
}



/*
 * LDY - Load Y Register 
 * Y,Z,N = M
 */
void CMOS6510::F_LDY(u8 addressmode){
	u16 address;
	u8 m;
	
	GetOperandAddress(addressmode, &address);	
	m = mMemory->Peek(address);

	CLRFLAG(FLAG_Z);
	CLRFLAG(FLAG_N);
	
		
	r_y = m;
	
	if(m & 0x80){
		SETFLAG(FLAG_N);		
	}
	if(m == 0){
		SETFLAG(FLAG_Z);		
	}
}



/*
 * LSR - Logical Shift Right 
 * A,C,Z,N = A/2 or M,C,Z,N = M/2 
 */
void CMOS6510::F_LSR(u8 addressmode){
	u16 address;
	u8 m;	
	
	CLRFLAG(FLAG_C);		
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		
	
	//Are we acting on memory or accummulator ?
	if(addressmode == ADDRESS_MODE_ACCUMULATOR){
		 m = r_a;		
	}else{	
		GetOperandAddress(addressmode, &address);
		m = mMemory->Peek(address);
	}

	if( (m & 0x01) > 0 ){
		SETFLAG(FLAG_C);
	}
	
	m = m >> 1;
	
	if( (m & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}
	if(m == 0){
		SETFLAG(FLAG_Z);
	}

	if(addressmode == ADDRESS_MODE_ACCUMULATOR){	
		r_a = m;	
	}else{
		mMemory->Poke(address, m);
	}
}


/*
 * NOP - No Operation
 */
void CMOS6510::F_NOP(u8 addressmode){
}


/*
 * ORA - Logical Inclusive OR 
 * A,Z,N = A|M 
 */
void CMOS6510::F_ORA(u8 addressmode){
	u16 address;
	u8 m;	
	
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		
	
	GetOperandAddress(addressmode, &address);
	m = mMemory->Peek(address);
	
	r_a = m | r_a;

	if(r_a == 0){
		SETFLAG(FLAG_Z);
	}
	if( (r_a & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}
	
	
//	mMemory->Poke(address, m);
}

/*
 * PHA - Push Accumulator
 */
void CMOS6510::F_PHA(u8 addressmode){
	Push(r_a);
}

/*
 * PHP - Push Processor Status 
 */
void CMOS6510::F_PHP(u8 addressmode){
	Push(r_p);
}


/*
 * PLA - Pull Accumulator 
 */
void CMOS6510::F_PLA(u8 addressmode){
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		

	r_a = Pop();

	if(r_a == 0){
		SETFLAG(FLAG_Z);
	}
	if( (r_a & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}

}


/*
 * PLP - Pull Processor Status
 */
void CMOS6510::F_PLP(u8 addressmode){
	r_p = Pop();
}


/*
 * ROL - Rotate Left
 */
void CMOS6510::F_ROL(u8 addressmode){
	u16 address;
	u8 m;	
	
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		
	
	//Are we acting on memory or accummulator ?
	if(addressmode == ADDRESS_MODE_ACCUMULATOR){
		 m = r_a;		
	}else{	
		GetOperandAddress(addressmode, &address);
		m = mMemory->Peek(address);
	}

	u8 oldbit = 0;
	if( (m & 0x80) > 0 ){
		oldbit = 1;
	}

	m = m << 1;

	if(ISFLAG(FLAG_C)){
		m = (m | 0x01);	
	}

	if(oldbit == 1){
		SETFLAG(FLAG_C);
	}else{
		CLRFLAG(FLAG_C);
	}
	if( (m & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}	
	if(m == 0){
		SETFLAG(FLAG_Z);
	}

	if(addressmode == ADDRESS_MODE_ACCUMULATOR){	
		r_a = m;	
	}else{
		mMemory->Poke(address, m);
	}
}

	
/*
 * ROR - Rotate Right 
 */
void CMOS6510::F_ROR(u8 addressmode){
	u16 address;
	u8 m;	
	
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		
	
	//Are we acting on memory or accummulator ?
	if(addressmode == ADDRESS_MODE_ACCUMULATOR){
		 m = r_a;		
	}else{	
		GetOperandAddress(addressmode, &address);
		m = mMemory->Peek(address);
	}

	u8 oldbit = 0;
	if( (m & 0x01) > 0 ){
		oldbit = 1;
	}

	m = m >> 1;

	if(ISFLAG(FLAG_C)){
		m = (m | 0x80);	
	}

	if(oldbit == 1){
		SETFLAG(FLAG_C);
	}else{
		CLRFLAG(FLAG_C);
	}
	if( (m & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}	
	if(m == 0){
		SETFLAG(FLAG_Z);
	}

	if(addressmode == ADDRESS_MODE_ACCUMULATOR){	
		r_a = m;	
	}else{
		mMemory->Poke(address, m);
	}
}


/*
 * RTI - Return from Interrupt
 */
void CMOS6510::F_RTI(u8 addressmode){
//cout << "rti" << endl;
	r_p = Pop();
	r_pc = Pop16();
}


/*
 * RTS - Return from Subroutine 
 */
void CMOS6510::F_RTS(u8 addressmode){
	r_pc = Pop16()+1;
}


/*
 * SBC - Subtract with Carry 
 * A,Z,C,N = A-M-(1-C)
 */
void CMOS6510::F_SBC(u8 addressmode){
	u16 address;
	u8 m;	
	s16 val, acalc;
	u8 sbit;
	
	GetOperandAddress(addressmode, &address);
	m = mMemory->Peek(address);

	acalc = r_a - m - ( 1 - ISFLAG(FLAG_C));
	
//cout << "m=0x"<< setw(2) << hex << (int)m;

	sbit = 0;
	if((r_a & 0x80) > 0){
		sbit = 1;
	}

	m = (~m);
//cout << ", ~m=0x"<< setw(2) << hex << (int)m;

	m = m + ISFLAG(FLAG_C);
//cout << ", m+C=0x"<< setw(2) << hex << (int)m;

	val = r_a + m;
//cout << ", val=0x"<< setw(4) << hex << (int)val;

	CLRFLAG(FLAG_V);		
	CLRFLAG(FLAG_C);	
	CLRFLAG(FLAG_N);	
	CLRFLAG(FLAG_Z);	

	
	r_a = (u8)val;

//cout << ", ra=0x"<< setw(2) << hex << (int)r_a;
//cout << "]";

		
	if( (r_a & 0x80) > 0){
		SETFLAG(FLAG_N);
	}
	if( r_a == 0 ){
		SETFLAG(FLAG_Z);
	}

//	if( (val & 0x100) > 0){  //Check This(!!!!)
	if(acalc >= 0 && acalc <= 0xFF){  //Check This(!!!!)
		SETFLAG(FLAG_C);
	}
//	if( !((sbit > 0 && (sbit & r_a) > 0) || (sbit == 0 && (sbit & r_a) == 0)) ){
	if(acalc < -128 || acalc > 127){
		SETFLAG(FLAG_V);
	}
}

/*
 * SEC - Set Carry Flag 
 * C = 1 
 */
void CMOS6510::F_SEC(u8 addressmode){
	SETFLAG(FLAG_C);
}


/*
 * SED - Set Decimal Flag
 */
void CMOS6510::F_SED(u8 addressmode){
cout << "SED" << endl;
	SETFLAG(FLAG_D);
}


/*
 * SEI - Set Interrupt Disable 
 */
void CMOS6510::F_SEI(u8 addressmode){
	SETFLAG(FLAG_I);
}


/*
 * STA - Store Accumulator
 */	
void CMOS6510::F_STA(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);
	
	mMemory->Poke(address, r_a);
}


/*
 * STX - Store X Register 
 */
void CMOS6510::F_STX(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);

	mMemory->Poke(address, r_x);
}


/*
 * STY - Store Y Register 
 */
void CMOS6510::F_STY(u8 addressmode){
	u16 address;
	
	GetOperandAddress(addressmode, &address);

	mMemory->Poke(address, r_y);
}


/*
 * TAX - Transfer Accumulator to X 
 */
void CMOS6510::F_TAX(u8 addressmode){
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		

	r_x = r_a;

	if(r_x == 0){
		SETFLAG(FLAG_Z);
	}
	if( (r_x & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}	
}


/*
 * TAY - Transfer Accumulator to Y 
 */	
void CMOS6510::F_TAY(u8 addressmode){
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		

	r_y = r_a;

	if(r_y == 0){
		SETFLAG(FLAG_Z);
	}
	if( (r_y & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}	
}


/*
 * TSX - Transfer Stack Pointer to X 
 */
void CMOS6510::F_TSX(u8 addressmode){
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		

	r_x = r_sp; //mMemory->Peek(STACKOFFSET + r_sp);

	if(r_x == 0){
		SETFLAG(FLAG_Z);
	}
	if( (r_x & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}		
}


/*
 * TXA - Transfer X to Accumulator 
 * @TODO: Check
 */
void CMOS6510::F_TXA(u8 addressmode){
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		

	r_a = r_x;

	if(r_a == 0){
		SETFLAG(FLAG_Z);
	}
	if( (r_a & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}		
}


/*
 * TXS - Transfer X to Stack Pointer
 * @TODO: Check
 */
void CMOS6510::F_TXS(u8 addressmode){
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		

	r_sp = r_x;

	if(r_sp == 0){
		SETFLAG(FLAG_Z);
	}
	if( (r_sp & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}		
}


/*
 * TYA - Transfer Y to Accumulator 
 */
void CMOS6510::F_TYA(u8 addressmode){
	CLRFLAG(FLAG_Z);		
	CLRFLAG(FLAG_N);		

	r_a = r_y;

	if(r_a == 0){
		SETFLAG(FLAG_Z);
	}
	if( (r_a & 0x80) > 0 ){
		SETFLAG(FLAG_N);
	}		
}



/*
 * Debug utput
 */
void CMOS6510::PrintOperands(u8 addressMode, u16 pc){
	
	
	switch(addressMode){
		case ADDRESS_MODE_ACCUMULATOR:
		case ADDRESS_MODE_IMPLIED:
			cout << "       ";
			break;
		case ADDRESS_MODE_IMMEDIATE:   //imm = #$00 ; (Immediate)
			cout <<  "#$" << hex << setw(2) << (int)mMemory->Peek(pc+1) << "   ";  
			break;
		case ADDRESS_MODE_ZEROPAGE:   //zp = $00 ; (Zero Page Absolute)
			cout <<  "$" << hex << setw(2) << (int)mMemory->Peek(pc+1) << "    ";  
			break;
		case ADDRESS_MODE_ZEROPAGE_X: //zpx = $00,X ; (Zero Page Index)
			cout <<  "$" << hex << setw(2) << (int)mMemory->Peek(pc+1) << ",X  ";  
			break;
		case ADDRESS_MODE_ZEROPAGE_Y: //zpy = $00,Y ; (Zero Page Index)
			cout <<  "$" << hex << setw(2) << (int)mMemory->Peek(pc+1) << ",Y  ";  
			break;
		case ADDRESS_MODE_INDIRECT_X:  //izx = ($00,X) ; ( Pre Index Indirect)
			cout <<  "($" << hex << setw(2) << (int)mMemory->Peek(pc+1) << ",X)";  
			break;
		case ADDRESS_MODE_INDIRECT_Y:  //izy = ($00),Y ; (Post Index Indirect)
			cout <<  "($" << hex << setw(2) << (int)mMemory->Peek(pc+1) << "),Y";  
			break;
		case ADDRESS_MODE_ABSOLUTE:  //abs = $0000 ; (Absolute)
			cout <<  "$" << hex << setw(4) << (int)mMemory->Peek16(pc+1) << "  ";   
			break;			
		case ADDRESS_MODE_ABSOLUTE_X: //abx = $0000,X ; (Index)
			cout <<  "$" << hex << setw(4) << (int)mMemory->Peek16(pc+1) << ",X";  
			break;
		case ADDRESS_MODE_ABSOLUTE_Y: //aby = $0000,Y ; (Index)
			cout <<  "$" << hex << setw(4) << (int)mMemory->Peek16(pc+1) << ",Y";  
			break;
		case ADDRESS_MODE_INDIRECT:// ind = ($0000) ; (Indirect) 
			cout <<  "($" << hex << setw(4) << (int)mMemory->Peek16(pc+1) << ")";  
			break;
		case ADDRESS_MODE_RELATIVE://rel = $0000 (PC-relative)
			cout <<  "$" << hex << setw(2) << (int)mMemory->Peek(pc+1) << "~   ";  
			break;	
		default:
			cout << "(?)    " ;
			break;
	}
}

void CMOS6510::PrintStatusBits(){
	cout << setw(1);
	cout << ISFLAG(FLAG_N);
	cout << ISFLAG(FLAG_V);
	cout << ISFLAG(0x20);
	cout << ISFLAG(FLAG_B);
	cout << ISFLAG(FLAG_D);
	cout << ISFLAG(FLAG_I);
	cout << ISFLAG(FLAG_Z);
	cout << ISFLAG(FLAG_C);
}



void CMOS6510::IRQ(){
	if(ISFLAG(FLAG_I)){
		return;
	}
	CLRFLAG(FLAG_B);
	Push16(r_pc);
	Push(r_p);
	SETFLAG(FLAG_I);
	r_pc=mBus->Peek16(0xFFFE);
//timer50Hz := timer50Hz+20
     
}

void CMOS6510::NMI(){
	CLRFLAG(FLAG_B);
	Push16(r_pc);
	Push(r_p);
	SETFLAG(FLAG_I);
	r_pc=mBus->Peek16(0xFFFA);
}