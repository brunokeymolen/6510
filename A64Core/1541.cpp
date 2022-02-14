/*
 *  1541.cpp
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 12/02/22.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "1541.h"


C1541::C1541() : mSerial(NULL) {

}


C1541::~C1541() {
}


void C1541::Init(SerialInterface* serial) {
    mSerial = serial;
    //hold data line to true to let cbm64 know we are listening
    SetSerialPin(SerialPin::DATA, 1);
}


void C1541::SerialEvent(SerialPin pin, u8 hilo) {
    switch (pin) {   
        case SerialPin::ATN:
            std::cerr << "serialevent ATN: " << " hi/lo: " << (int)hilo << std::endl;
            if (hilo == 1) {
                std::cerr << "ATN is pulled to True: release clk and pull the data line to let the processor known he 1541 is ready to listen" << std::endl;
                mState = SerialState::ATN;
                mLastEvent = mCycles;
            }
            break;
        case SerialPin::CLK:
            std::cerr << "serialevent CLK: " << " hi/lo: " << (int)hilo << std::endl;
            if (hilo == 0 && mState == SerialState::HANDSHAKE) {
                std::cerr << "ready for data" << std::endl;  
                mState = SerialState::READY_FOR_DATA;
                SetSerialPin(SerialPin::DATA, 0); //ready to talk

            }
            if (hilo == 1 && mState == SerialState::READY_FOR_DATA) {
                std::cerr << "talker took back clock" << std::endl;  
                mState = SerialState::RECEIVE;
            }
            break;
        case SerialPin::DATA:
            std::cerr << "serialevent DATA: " << " hi/lo: " << (int)hilo << std::endl;
            if (hilo == 0) {
                if (mState == SerialState::ATN) {
                    std::cerr << "expected to see this at ATN, Ready to receive" << std::endl;
                }
            }
            break;
    }
}


void C1541::SetSerialPin(SerialPin pin, u8 hilo) {
    if (mSerial) {
        mSerial->SetPin(pin, hilo);
    } else {
        std::cerr << "mSerial not set!" << std::endl;
    }
}


u8 C1541::GetSerialPin(SerialPin pin) {
    if (mSerial) {
        mSerial->GetPin(pin);
    }
    return 0xff;
}


void C1541::Cycle(uint64_t totalCycles) {
    mCycles = totalCycles;
    if (mState == SerialState::ATN && totalCycles - mLastEvent > 100) {
        std::cerr << "->receive" << std::endl;
        mState = SerialState::HANDSHAKE;
        SetSerialPin(SerialPin::CLK, 0);
        SetSerialPin(SerialPin::DATA, 1);
    }
}


u8 C1541::ReadByte() {
    return 0;
}
    

void C1541::SendByte(u8) {

}
 
