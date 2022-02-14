/*
 *  1541.h
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 12/02/22.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef C1541_H
#define C1541_H

#include "Common.h"
#include "serial.h"

class C1541 {
    private:
        enum SerialState {
            INIT,
            ATN,
            HANDSHAKE,
            READY_FOR_DATA,
            RECEIVE
        } mState;
    public:
    public:
        C1541();
        ~C1541();
	public:
        void Init(SerialInterface* serial);
        void SerialEvent(SerialPin pin, u8 hilo);
        void Cycle(uint64_t totalCycles);
    private:
        void SetSerialPin(SerialPin pin, u8 hilo);
        u8 GetSerialPin(SerialPin pin);
        u8 ReadByte();
        void SendByte(u8);
    private:
       SerialInterface *mSerial; 
       uint64_t mCycles;
       uint64_t mLastEvent;
};


#endif 
