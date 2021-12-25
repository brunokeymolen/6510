/*
 *  MOS6510Debug.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 08/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

// PC=FCE2 IR=A2 A=00 X=FF Y=00 S=00 P=10000000

#include "MOS6510Debug.h"

#include <ctype.h>      
#include <math.h> 

int htoi(char str[])
{
    int value = 0,  /* the sum of the hexadecimal digits' equivalent
                       decimal values */
           weight,  /* the weight of a hexadecimal digit place */   
            digit,
            i = 0,  /* array index */
              len,  /* length of string */
                c;  /* character */            
   
    /* report error on empty string */
    if (str[i] == '\0')
        return -1;
        
    /* ignore 0x or 0X in hexidecimal string */
    if (str[i] == '0') {
        ++i;
        if (str[i] == 'x' || str[i] == 'X')
            ++i;
    }
    
    len = strlen(str);    
    
    /* calculate each hex character's decimal value */
    for ( ; i < len; ++i) {
        c = tolower(str[i]);
        if (c >= '0' && c <= '9')
            digit = c - '0';
        else if (c >= 'a' && c <= 'f') {
            switch (c) {
                case 'a':
                    digit = 10;
                    break;
                case 'b':
                    digit = 11;
                    break;
                case 'c':
                    digit = 12;
                    break;
                case 'd':
                    digit = 13;
                    break;
                case 'e':
                    digit = 14;
                    break;
                case 'f':
                    digit = 15;
                    break;
                default:
                    break;
            }
        }
        else
            return -1;  /* invalid input encountered */
        
        weight = (int) pow(16, (double) (len - i - 1));
        value += weight * digit;
    }
    return value;
}


CMOS6510Debug::CMOS6510Debug(){
	mBufLen = 512;
	mBuf = new char[mBufLen];

	mStream = new fstream();
	mStream->open("/Users/bruno/Projects/A64/Data/ED64doc Build an Emulator/ed64Chap3/sin63.log",ios::in);	
	mStream->getline(mBuf, mBufLen);
	mStream->getline(mBuf, mBufLen);
	mStream->getline(mBuf, mBufLen);
	mStream->getline(mBuf, mBufLen);

}


CMOS6510Debug::~CMOS6510Debug(){
	mStream->close();
	delete[] mBuf;
}

void CMOS6510Debug::SkipTraceLine(){
	mStream->getline(mBuf, mBufLen);
}

int CMOS6510Debug::GetTraceLine(u16* pc, u8* opcode, u8* a, u8* x, u8* y, u8* sp, u8* p){
	mStream->getline(mBuf, mBufLen);
	
	
	char* oriBuf = mBuf;
	mBuf = mBuf + 11;
	
	mBuf[44] = 0;
	
//	#ifdef DEBUG_TRACE_OPCODE
		cout <<  mBuf << " :: " ;
//	#endif

	// 0         1         2         3         4
	// 01234567890123456789012345678901234567890123 
	// PC=FCE2 IR=A2 A=00 X=FF Y=00 S=00 P=10000000
	
	// 0009C82C - PC=E5CF IR=85 A=00 X=00 Y=01 S=F3 P=00000011
	
	mBuf[7] = mBuf[13] = mBuf[18] = mBuf[23] = mBuf[28] = mBuf[33] = 0;
	mBuf[2] = mBuf[10] = mBuf[15] = mBuf[20] = mBuf[25] = mBuf[30] = 'x';
	mBuf[1] = mBuf[9] = mBuf[14] = mBuf[19] = mBuf[24] = mBuf[29] = '0';
	
	*pc = htoi(&mBuf[1]);
	*opcode = htoi(&mBuf[9]);
	*a = htoi(&mBuf[14]);
	*x = htoi(&mBuf[19]);
	*y = htoi(&mBuf[24]);
	*sp = htoi(&mBuf[29]);
	*p = 0x20;
	if(mBuf[36] == '1'){
		*p = *p | 0x80;
	}
	if(mBuf[37] == '1'){
		*p = *p | 0x40;
	}
	if(mBuf[39] == '1'){
		*p = *p | 0x10;
	}
	if(mBuf[40] == '1'){
		*p = *p | 0x08;
	}
	if(mBuf[41] == '1'){
		*p = *p | 0x04;
	}
	if(mBuf[42] == '1'){
		*p = *p | 0x02;
	}
	if(mBuf[43] == '1'){
		*p = *p | 0x01;
	}

	mBuf = oriBuf;
		
	
	return 0;
}
