/*
 * =====================================================================================
 *
 *       Filename:  serial.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/12/22 18:45:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef A64_SERIAL_H
#define A64_SERIAL_H

#include "Common.h"

enum class SerialPin {
    SRQIN,
    GND,
    ATN,
    CLK,
    DATA,
    RESET
};

class SerialInterface {
public:
    virtual void SetPin(SerialPin pin, u8 hilo) = 0;
    virtual u8 GetPin(SerialPin pin) = 0;
};



#endif
