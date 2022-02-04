/*
 *  MOS6510.h
 *  A64Emulator
 *
 *  6510 Microprocesor Emulator
 * 
 *  Created by Bruno Keymolen on 05/06/08.
 *  Copyright 2008 Arcanegra bvba. All rights reserved.
 *
 */

#ifndef MOS6510
#define MOS6510

#include "Common.h"
#include "MOS6510Debug.h"
#include "Bus.h"
#include "General.h"



class CMOS6510;

using namespace std;

/*
 * Processor Status Register Flags
 *
 * +---+---+---+---+---+---+---+---+
 * | N | V | 1 | B | D | I | Z | C |
 * +---+---+---+---+---+---+---+---+
 *   |   |       |   |   |   |    \_Carry
 *   |   |       |   |   |    \_Zero
 *   |   |       |   |    \_Interupt enable/disable flag
 *   |   |       |    \_Decimal Mode
 *   |   |        \_BRK 
 *   |    \_Overflow 
 *    \_Negative (Sign)
 */
#define FLAG_N (u8)0x80 
#define FLAG_V (u8)0x40 
#define FLAG_B (u8)0x10 
#define FLAG_D (u8)0x08
#define FLAG_I (u8)0x04
#define FLAG_Z (u8)0x02 
#define FLAG_C (u8)0x01 

#define RESETFLAGS r_p=r_p&(u8)0x20 
#define ISFLAG(f) ( (r_p & f)==f?1:0)
#define SETFLAG(f) r_p=r_p|f
#define CLRFLAG(f) r_p=(r_p & f)==f?r_p^f:r_p


/*
 * Stack
 */
#define STACKOFFSET 0x0100 


/*
 * Addressing Modes
 */ 
typedef const enum _eMos6502AddressMode{
	ADDRESS_MODE_UNKNOWN			= -1,
	ADDRESS_MODE_IMMEDIATE			= 0, 
	ADDRESS_MODE_ZEROPAGE			= 1,
	ADDRESS_MODE_ZEROPAGE_X			= 2,
	ADDRESS_MODE_ZEROPAGE_Y			= 3,
	ADDRESS_MODE_IMPLIED			= 4,
	ADDRESS_MODE_ACCUMULATOR		= 5,
	ADDRESS_MODE_ABSOLUTE			= 6, 
	ADDRESS_MODE_ABSOLUTE_X			= 7,
	ADDRESS_MODE_ABSOLUTE_Y			= 8,
	ADDRESS_MODE_INDIRECT			= 9,
	ADDRESS_MODE_INDIRECT_X			= 10,
	ADDRESS_MODE_INDIRECT_Y			= 11,
	ADDRESS_MODE_RELATIVE			= 12,
	ADDRESS_MODE_MAX				= 13
}eMos6502AddressMode, Mos6502AddressMode;


/*
 * Instructions
 */
typedef const enum _MOS6502Instruction{
	ADC = 0,
	AND,
	ASL,
	BCC,
	BCS,
	BEQ,
	BIT,
	BMI,
	BNE,
	BPL,
	BRK,
	BVC,
	BVS,
	CLC,
	CLD,
	CLI,
	CLV,
	CMP,
	CPX,
	CPY,
	DEC,
	DEX,
	DEY,
	EOR,
	INC,
	INX,
	INY,
	JMP,
	JSR,
	LDA,
	LDX,
	LDY,
	LSR,
	NOP,
	ORA,
	PHA,
	PHP,
	PLA,
	PLP,
	ROL,
	ROR,
	RTI,
	RTS,
	SBC,
	SEC,
	SED,
	SEI,
	STA,
	STX,
	STY,
	TAX,
	TAY,
	TSX,
	TXA,
	TXS,
	TYA,
	END_OPC,
	ILLEGAL_OPC,
	_______BROLBROL,
}eMOS6502Instruction, MOS6502Instruction;


/*
 * Opcodes
 */
typedef struct _MOS6502Opcodes{
	MOS6502Instruction ID;
	u8 oppcodes[ADDRESS_MODE_MAX]; //order see eMos6502AddressMode
	char assembly[4];
	char description[50];
}MOS6502Opcodes;

typedef struct _MOS6502Cycles{
	MOS6502Instruction ID;
	int cycles[ADDRESS_MODE_MAX]; //order see eMos6502AddressMode
}MOS6502Cycles;


#define _x__ 0x02

typedef struct _MOS6502OpcodesLinear{
	u8 matrixID; //MOS6502Instruction
	u8 addressMode; //Mos6502AddressMode
	char* assembly;
	char* info;
	void(CMOS6510::*opcodeFunction)(u8); //Mos6502AddressMode		
    int cycles;
}MOS6502OpcodesLinear;

#define OPCODESMAX 256


/*
 * Little & Big Endian Conversions
 */
//#define ENDIAN LITTLE 
//#define NTOHS(ne) 
//#define HTONS(he)


/*
 * Processor simulator
 */
class CMOS6510{
private:
	u16 r_pc;
	u8	r_sp;
	u8	r_a;
	u8	r_x;
	u8	r_y;
	u8	r_p; //Processor Status register 

    uint64_t _cycles = 0;
    long mips;
    long mipsactive;

    int timeNow, prevIrTime, startMips;
    bool ir;

	CBus* mBus;
//	CMemory *mMemory;
	CBus *mMemory;
	MOS6502OpcodesLinear mOpcodes[OPCODESMAX];

	CMOS6510Debug* mOpcodeDebug;

	void PrintOperands(u8 addressMode, u16 pc);
	void PrintStatusBits();

	//BKE_MUTEX mMutex;
	CHiresTime* mHiresTime;

//#ifdef DEBUG_CONSISTENCY_CHECK	
	int mOpCnt;
//#endif
    
    char* opnull;
protected:
public:
	int mDisassemble;

//	CMOS6510(CMemory* mem);
	CMOS6510(BKE_MUTEX mutex);
	~CMOS6510();
	void Run();
    int Tick();
	//Stack
	void Push(u8);
	void Push16(u16);
	u8 Pop();
	u16 Pop16();
	//PC	
	void SetPC(u16 address);	
	bool DBGRunOneInstruction(u16* pc, u16* sp, u8* a, u8* x, u8* y, u8* p);
	void DBGTraceLine(u8 cmd, u16 prevPC);
	void SetHiresTime(CHiresTime* hiresTime);
    
    uint64_t GetCycles();

private:
	bool GetOperandAddress(u8 addressMode, u16* address);
	void IRQ();
	void NMI();
	
	//------------------------------------
	// Opcode Implementations
	//------------------------------------
	void F_ADC(u8 addressmode);
	void F_AND(u8 addressmode);
	void F_ASL(u8 addressmode);
	void F_BCC(u8 addressmode);
	void F_BCS(u8 addressmode);
	void F_BEQ(u8 addressmode);	
	void F_BIT(u8 addressmode);
	void F_BMI(u8 addressmode);
	void F_BNE(u8 addressmode);
	void F_BPL(u8 addressmode);
	void F_BRK(u8 addressmode);
	void F_BVC(u8 addressmode);
	void F_BVS(u8 addressmode);
	void F_CLC(u8 addressmode);
	void F_CLD(u8 addressmode);
	void F_CLI(u8 addressmode);
	void F_CLV(u8 addressmode);
	void F_CMP(u8 addressmode);
	void F_CPX(u8 addressmode);
	void F_CPY(u8 addressmode);
	void F_DEC(u8 addressmode);
	void F_DEX(u8 addressmode);
	void F_DEY(u8 addressmode);
	void F_EOR(u8 addressmode);
	void F_INC(u8 addressmode);
	void F_INX(u8 addressmode);
	void F_INY(u8 addressmode);
	void F_JMP(u8 addressmode);
	void F_JSR(u8 addressmode);
	void F_LDA(u8 addressmode);
	void F_LDX(u8 addressmode);
	void F_LDY(u8 addressmode);
	void F_LSR(u8 addressmode);
	void F_NOP(u8 addressmode);
	void F_ORA(u8 addressmode);
	void F_PHA(u8 addressmode);
	void F_PHP(u8 addressmode);
	void F_PLA(u8 addressmode);
	void F_PLP(u8 addressmode);
	void F_ROL(u8 addressmode);
	void F_ROR(u8 addressmode);
	void F_RTI(u8 addressmode);
	void F_RTS(u8 addressmode);
	void F_SBC(u8 addressmode);
	void F_SEC(u8 addressmode);
	void F_SED(u8 addressmode);
	void F_SEI(u8 addressmode);
	void F_STA(u8 addressmode);
	void F_STX(u8 addressmode);
	void F_STY(u8 addressmode);
	void F_TAX(u8 addressmode);
	void F_TAY(u8 addressmode);
	void F_TSX(u8 addressmode);
	void F_TXA(u8 addressmode);
	void F_TXS(u8 addressmode);
	void F_TYA(u8 addressmode);
private:
    

};

#endif // MOS6510

