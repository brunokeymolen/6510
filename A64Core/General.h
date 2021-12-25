/*
 *  General.h
 *  A64Mac
 *
 *  Created by Bruno Keymolen on 22/07/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef GENERAL_H
#define GENERAL_H

#define LOCATION_ROMS "/Users/bruno/Projects/A64/A64Mac/roms/"

class CHiresTime {
public:
	virtual double GetMicroseconds() = 0;
	virtual int GetMicrosecondsLo() = 0;
};

#endif //GENERAL_H