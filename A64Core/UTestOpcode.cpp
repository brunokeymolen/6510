/*
 *  UTestOpcode.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 16/06/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "UTestOpcode.h"
#include <cstdlib> 
#include <ctime> 
#include <iostream>

CUTestOpcode::CUTestOpcode(CMOS6510* processor, CBus* bus){
	mProcessor = processor;
	mMemory = bus;
	
	mBaseAddress = 0x1000;
}

CUTestOpcode::~CUTestOpcode(){
}

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

#define DBG_ISFLAG(reg, f) ((reg & f)==f?1:0)
#define DBG_SETFLAG(reg, f) reg=reg | f

/* 
 * ADC - Add memory to accumulator with carry
 * T1 = C=0 ; 0x10 + 0x20 = 0x30 ; 16 + 32 = 48; 00010000 + 00100000 =  00110000 ; N=0, C=0, Z=0
 * T2 = C=0 ; 0xF0 + 0x20 = 0x0190 ; 240 + 32 = 272; 11110000 + 00100000 =  100010000 ; N=1, C=1, Z=0
 * T3 = C=0 ; 0x80 + 0x80 = 0x0100 ; 128 + 128 = 256; 10000000 + 10000000 =  100000000 ; N=0, C=1, Z=1
 *
 * A,Z,C,N = A+M+C 
 */
//	 * 0xF0 + 0x20 = 0x0190 ; -112 + 32 = -75; 11110000 + 00100000 =  100010000 ; N=0, C=0, Z=0
int CUTestOpcode::Test_ADC(u8 opcode, u8 A, u8 M, u8 C, char* info){
	int ret;
	u8 a, x, y, p;
	u16 pc, sp;

	x, y, p = 0; //clear registers
	sp = 0xFF; //set stack pointer
	cout << setfill('0') << uppercase;
	cout <<  __FUNCTION__ << " - 0x" << setw(2) << hex << (int)opcode << ", " << info << " : ";

	srand((unsigned)time(0)); 

	a = A; //Copy accumulator so we keep the original one
	
	if(C == 1){
		DBG_SETFLAG(p, FLAG_C);
	}


	switch(opcode){
		case 0x69: //Immediate
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, M); 
			break;
		case 0x65: //Zero Page
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x10); 
			mMemory->Poke(0x0010, M); 
			break;
		case 0x75: //Zero Page, X
			x = ((u8)(rand()%0xFF))+1;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x80); 
			mMemory->Poke( (u16)((u8)(0x80 + x)), M); 
			break;
		case 0x6D: //Absolute
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x50); 
			mMemory->Poke(mBaseAddress + 0x02, 0x45); 
			mMemory->Poke(0x4550, M); 
			break;
		case 0x7D: //Absolute, X
			x = ((u8)(rand()%0xFF))+1;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x00); 
			mMemory->Poke(mBaseAddress + 0x02, 0x40); 
			mMemory->Poke(0x4000 + x, M); 
			break;
		case 0x79: //Absolute, Y
			y = ((u8)(rand()%0xFF))+1;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x00); 
			mMemory->Poke(mBaseAddress + 0x02, 0x40); 
			mMemory->Poke(0x4000 + y, M); 
			break;
		case 0x61: //(Indirect, X)
			x = ((u8)(rand()%0x80))+1;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x40 ); 
			mMemory->Poke( (u8)(0x40 + x), 0x00);
			mMemory->Poke( (u8)(0x40 + x)+1, 0x50);
			mMemory->Poke(0x5000, M); 
		case 0x71: //(Indirect), Y
			y = ((u8)(rand()%0xFF))+1;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x40 ); 
			mMemory->Poke( (u8)0x40, 0x00);
			mMemory->Poke( (u8)0x41, 0x50);
			mMemory->Poke(0x5000 + y, M); 
			 
			break;
		
		default:
			ret = - 100;
			goto ADC_0x69_EXIT;	
	}

	pc = mBaseAddress;
	ret = mProcessor->DBGRunOneInstruction(&pc, &sp, &a, &x, &y, &p);

	//Check
	ret = 0;
	
	u16 res = A + M + C;
	
	//Accumulator
	if(a == (u8)res){
		ret = 0; //ok
	}else{
		ret = -1;
		goto ADC_0x69_EXIT;
	}
	
	
	//Zero
	if(res == 0 && (DBG_ISFLAG(p, FLAG_Z) == 1) ){
		ret = 0; //ok
	}else if(res != 0 && (DBG_ISFLAG(p, FLAG_Z) == 0) ){
		ret = 0; //ok
	}else{
		ret = -2;
		goto ADC_0x69_EXIT;
	}
	
	//Carry Flag
	if(res > 0xFF && (DBG_ISFLAG(p, FLAG_C)) ){
		ret = 0; //ok
	}else if(res <= 0xFF && !(DBG_ISFLAG(p, FLAG_C)) ){
		ret = 0; //ok
	}else{
		ret = -3;
		goto ADC_0x69_EXIT;
	}
	
	//Negative
	if((s8)res < 0 && (DBG_ISFLAG(p, FLAG_N)) ){
		ret = 0; //ok
	}else if((s8)res >= 0 && !(DBG_ISFLAG(p, FLAG_N)) ){
		ret = 0; //ok
	}else{
		ret = -4;
		goto ADC_0x69_EXIT;
	}
	
	
	
ADC_0x69_EXIT:
	if(ret == 0){
		cout << "Ok." << endl;	
	}else{
		cout << "Fail (" << dec << ret << ")! " << endl;
	}
	return ret;
}


/* 
 * JMP - Jump
 */
int CUTestOpcode::Test_JMP(u8 opcode, u8 toHB, u8 toLB, char* info){
	int ret;

	u8 a, x, y, p;
	u16 pc, sp;

	x, y, p = 0; //clear registers
	sp = 0x1FF; //set stack pointer
	cout << setfill('0') << uppercase;
	cout <<  __FUNCTION__ << " - 0x" << setw(2) << hex << (int)opcode << ", " << info << " : ";

	srand((unsigned)time(0)); 

	switch(opcode){
		case 0x4C: //Absolute
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, toLB); 
			mMemory->Poke(mBaseAddress + 0x02, toHB); 
			break;
		case 0x6C: //Indirect
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x50); 
			mMemory->Poke(mBaseAddress + 0x02, 0x40); 
			mMemory->Poke(0x4050, toLB); 
			mMemory->Poke(0x4051, toHB); 
			break;
		default:
			ret = - 100;
			goto JMP_EXIT;	
	}	
	
	pc = mBaseAddress;
	ret = mProcessor->DBGRunOneInstruction(&pc, &sp, &a, &x, &y, &p);
	
	//Check PC
	u8 pcLB = (pc & 0x00FF);
	u8 pcHB = ((pc & 0xFF00) >> 8);		
	
	ret = 0;
	
	if(pcLB != toLB){
		ret = -1;
		goto JMP_EXIT;
	}
	if(pcHB != toHB){
		ret = -2;
		goto JMP_EXIT;
	}
	
	
JMP_EXIT:
	if(ret == 0){
		cout << "Ok." << endl;	
	}else{
		cout << "Fail (" << dec << ret << ")! " << endl;
	}
	return ret;
}


/* 
 * SBC - Subtract with Carry 
 */
int CUTestOpcode::Test_SBC(u8 opcode, u8 A, u8 M, u8 C, u8 resA, u8 resP, char* info){
	int ret;

	u8 a, x, y, p;
	u16 pc, sp;

	x, y, p = 0; //clear registers
	sp = 0x1FF; //set stack pointer
	cout << setfill('0') << uppercase;
	cout <<  __FUNCTION__ << " - 0x" << setw(2) << hex << (int)opcode << ", " << info << " : ";
	
	a = A;

	if(C == 1){
		DBG_SETFLAG(p, FLAG_C);
	}

	srand((unsigned)time(0)); 

	switch(opcode){
		case 0xE9: //Immediate
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, M); 
			break;
		case 0xE5: //Zero Page
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x15); 
			mMemory->Poke(0x0015, M); 
			break;
		case 0xF5: //Zero Page, X
			x = ((u8)(rand()%0xF0))+0x0F;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x80); 
			mMemory->Poke( (u16)((u8)(0x80 + x)), M); 
			break;
		case 0xED: //Absolute
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x50); 
			mMemory->Poke(mBaseAddress + 0x02, 0x45); 
			mMemory->Poke(0x4550, M); 
			break;
		case 0xFD: //Absolute, X
			x = ((u8)(rand()%0xFF))+1;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x00); 
			mMemory->Poke(mBaseAddress + 0x02, 0x40); 
			mMemory->Poke(0x4000 + x, M); 
			break;
		case 0xF9: //Absolute, Y
			y = ((u8)(rand()%0xF0))+0x0F;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x00); 
			mMemory->Poke(mBaseAddress + 0x02, 0x40); 
			mMemory->Poke(0x4000 + y, M); 
			break;
		case 0xE1: //(Indirect, X)
			x = ((u8)(rand()%0x80))+1;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x40 ); 
			mMemory->Poke( (u8)(0x40 + x), 0x00);
			mMemory->Poke( (u8)(0x40 + x)+1, 0x50);
			mMemory->Poke(0x5000, M); 
		case 0xF1: //(Indirect), Y
			y = ((u8)(rand()%0xFF))+1;
			mMemory->Poke(mBaseAddress + 0x00, opcode); 
			mMemory->Poke(mBaseAddress + 0x01, 0x40 ); 
			mMemory->Poke( (u8)0x40, 0x00);
			mMemory->Poke( (u8)0x41, 0x50);
			mMemory->Poke(0x5000 + y, M); 
			 
			break;
		
		default:
			ret = - 100;
			goto SBC_EXIT;	
	}

	pc = mBaseAddress;
	ret = mProcessor->DBGRunOneInstruction(&pc, &sp, &a, &x, &y, &p);
	
	ret = 0;
		
	if(a == resA && p == resP){
		ret = 0; //ok
	}else{
		ret = -1;
		goto SBC_EXIT;
	}

SBC_EXIT:
	if(ret == 0){
		cout << "Ok." << endl;	
	}else{
		cout << "Fail (" << dec << ret << ")! " << endl;
	}
	return ret;
}



int CUTestOpcode::StartTest(){
	int ret;
	
	/*
	 * ADC - Add memory to accumulator with carry 
	 */
	 
	//T1
	ret = 0;
	cout << endl;
	////Opcode, A, M, C, Info
	ret = ret + Test_ADC(0x69, 0x10, 0x20, 0, "Immediate Adressing - Test 1.1.1"); 
	ret = ret + Test_ADC(0x69, 0xF0, 0x20, 0, "Immediate Adressing - Test 1.1.2"); 
	ret = ret + Test_ADC(0x69, 0xF0, 0x20, 1, "Immediate Adressing - Test 1.1.3"); 
	ret = ret + Test_ADC(0x69, 0xF0, 0xE0, 1, "Immediate Adressing - Test 1.1.4"); 
	ret = ret + Test_ADC(0x69, 0x00, 0x00, 0, "Immediate Adressing - Test 1.1.5"); 
	ret = ret + Test_ADC(0x69, 5, (u8)-20, 0, "Immediate Adressing - Test 1.1.6"); 
	//T2
	cout << endl;
	ret = ret + Test_ADC(0x65, 0x10, 0x20, 0, "Zero Page Adressing - Test 1.2.1"); 
	ret = ret + Test_ADC(0x65, 0xF0, 0x20, 0, "Zero Page Adressing - Test 1.2.2"); 
	ret = ret + Test_ADC(0x65, 0xF0, 0x20, 1, "Zero Page Adressing - Test 1.2.3"); 
	ret = ret + Test_ADC(0x65, 0xF0, 0xE0, 1, "Zero Page Adressing - Test 1.2.4"); 
	ret = ret + Test_ADC(0x65, 0x00, 0x00, 0, "Zero Page Adressing - Test 1.2.5"); 
	ret = ret + Test_ADC(0x65, 35, (u8)-20, 0, "Zero Page Adressing - Test 1.2.6"); 
	//T3
	cout << endl;
	ret = ret + Test_ADC(0x75, 0x40, 0x20, 0, "Zero Page, X Adressing - Test 1.3.1"); 
	ret = ret + Test_ADC(0x75, 0xF0, 0x20, 0, "Zero Page, X Adressing - Test 1.3.2"); 
	ret = ret + Test_ADC(0x75, 0xF0, 0x20, 1, "Zero Page, X Adressing - Test 1.3.3"); 
	ret = ret + Test_ADC(0x75, 0xF0, 0xE0, 1, "Zero Page, X Adressing - Test 1.3.4"); 
	ret = ret + Test_ADC(0x75, 0x00, 0x00, 0, "Zero Page, X Adressing - Test 1.3.5"); 
	ret = ret + Test_ADC(0x75, (u8)-6, (u8)-122, 0, "Zero Page, X Adressing - Test 1.3.6"); 
	//T4
	cout << endl;
	ret = ret + Test_ADC(0x6D, 0x15, 0x20, 0, "Absolute Adressing - Test 1.4.1"); 
	ret = ret + Test_ADC(0x6D, 0xF0, 0x20, 0, "Absolute Adressing - Test 1.4.2"); 
	ret = ret + Test_ADC(0x6D, 0xF0, 0x20, 1, "Absolute Adressing - Test 1.4.3"); 
	ret = ret + Test_ADC(0x6D, 0xF0, 0xE0, 1, "Absolute Adressing - Test 1.4.4"); 
	ret = ret + Test_ADC(0x6D, 0x00, 0x00, 0, "Absolute Adressing - Test 1.4.5"); 
	ret = ret + Test_ADC(0x6D, 0x00, 0x00, 0, "Absolute Adressing - Test 1.4.5"); 
	ret = ret + Test_ADC(0x6D, (u8)-7, (u8)-122, 1, "Absolute Adressing - Test 1.4.6"); 
	//T5
	cout << endl;
	ret = ret + Test_ADC(0x7D, 0x15, 0x20, 0, "Absolute Adressing, X - Test 1.5.1"); 
	ret = ret + Test_ADC(0x7D, 0xF0, 0x20, 0, "Absolute Adressing, X - Test 1.5.2"); 
	ret = ret + Test_ADC(0x7D, 0xF0, 0x20, 1, "Absolute Adressing, X - Test 1.5.3"); 
	ret = ret + Test_ADC(0x7D, 0xF0, 0xE0, 1, "Absolute Adressing, X - Test 1.5.4"); 
	ret = ret + Test_ADC(0x7D, 0x00, 0x00, 0, "Absolute Adressing, X - Test 1.5.5"); 
	//T5
	cout << endl;
	ret = ret + Test_ADC(0x79, 0x15, 0x20, 0, "Absolute Adressing, Y - Test 1"); 
	ret = ret + Test_ADC(0x79, 0xF0, 0x20, 0, "Absolute Adressing, Y - Test 2"); 
	ret = ret + Test_ADC(0x79, 0xF0, 0x20, 1, "Absolute Adressing, Y - Test 3"); 
	ret = ret + Test_ADC(0x79, 0xF0, 0xE0, 1, "Absolute Adressing, Y - Test 4"); 
	ret = ret + Test_ADC(0x79, 0x00, 0x00, 0, "Absolute Adressing, Y - Test 5"); 
	//T6
	cout << endl;
	ret = ret + Test_ADC(0x61, 0x15, 0x20, 0, "(Indirect, X) Adressing - Test 1"); 
	ret = ret + Test_ADC(0x61, 0xF0, 0x20, 0, "(Indirect, X) Adressing - Test 2"); 
	ret = ret + Test_ADC(0x61, 0xF0, 0x20, 1, "(Indirect, X) Adressing - Test 3"); 
	ret = ret + Test_ADC(0x61, 0xF0, 0xE0, 1, "(Indirect, X) Adressing - Test 4"); 
	ret = ret + Test_ADC(0x61, 0x00, 0x00, 0, "(Indirect, X) Adressing - Test 5"); 
	//T7
	cout << endl;
	ret = ret + Test_ADC(0x71, 0x15, 0x20, 0, "(Indirect), Y Adressing - Test 1"); 
	ret = ret + Test_ADC(0x71, 0xF0, 0x20, 0, "(Indirect), Y Adressing - Test 2"); 
	ret = ret + Test_ADC(0x71, 0xF0, 0x20, 1, "(Indirect), Y Adressing - Test 3"); 
	ret = ret + Test_ADC(0x71, 0xF0, 0xE0, 1, "(Indirect), Y Adressing - Test 2"); 
	ret = ret + Test_ADC(0x71, 0x00, 0x00, 0, "(Indirect), Y Adressing - Test 1"); 


	/*
	 * JMP - Jump to New Location 
	 */
	
	//Test_JMP(u8 opcode, u8 toHB, u8 toLB, char* info);
	cout << endl;
	ret = ret + Test_JMP(0x4C, 0x36, 0x25, "Absolute - Test 1"); 
	ret = ret + Test_JMP(0x4C, 0xFF, 0x00, "Absolute - Test 2"); 
	ret = ret + Test_JMP(0x4C, 0x00, 0xFF, "Absolute - Test 3"); 

	cout << endl;
	ret = ret + Test_JMP(0x6C, 0x36, 0x25, "Indirect - Test 1"); 
	ret = ret + Test_JMP(0x6C, 0xFF, 0x00, "Indirect - Test 2"); 
	ret = ret + Test_JMP(0x6C, 0x00, 0xFF, "Indirect - Test 3"); 


	/* 
	 * SBC - Subtract with Carry 
	 */
	cout << endl;
	////Opcode, A, M, C, resA, resP, Info
	ret = ret + Test_SBC(0xE9, 5, 3, 1, 2, 0x01, "Immediate Adressing - Test 1"); 
	ret = ret + Test_SBC(0xE9, 160, 8, 1, 152, 0x0, "Immediate Adressing - Test 2"); 



	if(ret == 0){
		cout << endl << endl << "All Tests Successful." << endl;
	}else{
		cout << endl << endl << "Tests finished with errors!" << endl;
	}
			
	return 0;
}