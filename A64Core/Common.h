/*
 *  Common.h
 *  A64Emulator
 *
 *  Created by Bruno Keymolen on 04/06/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */



#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <iostream>
#include <iomanip>
#include "bkegen.h"
#include <pthread.h>
#include <iomanip>
#include <sys/time.h>
#include <stdio.h>
#include <iomanip>
#include <unistd.h>	


namespace common {

// https://retrotinker.net/who-turned-the-clock/ 
#if 1
    //NTSC
    static const int CPUHZ = 1022730;
#else
    //PAL
    static const int CPUHZ = 985270;
#endif

std::string binary(u8 v);


} //ns
#endif
