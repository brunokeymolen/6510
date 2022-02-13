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
                mState = SerialState::ATN;
                SetSerialPin(SerialPin::CLK, 0);
                SetSerialPin(SerialPin::DATA, 1);
            }
            break;
        case SerialPin::CLK:
            std::cerr << "serialevent CLK: " << " hi/lo: " << (int)hilo << std::endl;
            if (hilo) {
                std::cerr << "initial situation" << std::endl;  
            //    SetSerialPin(SerialPin::DATA, 1);
                
            }
            if (hilo == 0) {
                //To begin the talker releases the Clock line to false
           //     SetSerialPin(SerialPin::DATA, 0); //We are ready to talk
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

}


u8 C1541::ReadByte() {
    return 0;
}
    

void C1541::SendByte(u8) {

}
 
