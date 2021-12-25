//
//  C64CocoaInit.h
//  A64Mac
//
//  Created by Bruno Keymolen on 12/07/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#ifndef C64COCOA_H
#define C64COCOA_H

#import <Cocoa/Cocoa.h>


@interface C64Cocoa : NSObject {
		void* mCBM64;
}

-(void)InitWithView:(NSView*)view;
-(void)Start;
-(int)AddKeyStroke:(char)c;

@end


#endif //C64COCOA_H