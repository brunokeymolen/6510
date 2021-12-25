/* VICView */

#import <Cocoa/Cocoa.h>
#import "C64Cocoa.h"


@interface VICView : NSView
{
	C64Cocoa* mC64;
	unsigned char* mCharMem; //80 x 25
	unsigned char* mScreenBuffer; //320 x 200
	
	NSColor* m_CBM_COLOR_BLUE_6;
	NSColor* m_CBM_COLOR_LIGHT_BLUE_14;
}

-(void) SetScreenBuffer:(unsigned char*)screenbuffer; //320 x 200

-(void) DrawCharAt:(int)address character:(char)c;
-(void) DrawCharsFromMemory:(unsigned char*)memory;
-(void) DrawPixelsFromBuffer:(unsigned char*)screenBuffer X:(int)x Y:(int)y Width:(int)w Height:(int)h;

-(BOOL)acceptsFirstResponder;
-(void)keyDown:(NSEvent*)event;


@end
