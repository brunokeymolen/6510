#import "VICView.h"
#import "Timer.h"


@implementation VICView




- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil) {
		
		m_CBM_COLOR_LIGHT_BLUE_14 = [[NSColor colorWithCalibratedRed:(154.0/255.0) green:(145.0/255) blue:(221.0/255.0) alpha:1.0] retain];
		m_CBM_COLOR_BLUE_6 = [[NSColor colorWithDeviceRed:(78.0/255.0) green:(69.0/255.0) blue:(152.0/255.0) alpha:1.0] retain];

		mC64 = [[C64Cocoa alloc] init];
		[mC64 InitWithView:self];
		[mC64 Start]; 
		mCharMem = NULL;
	}
	return self;
}


//320 x 200
-(void) SetScreenBuffer:(unsigned char*)screenbuffer{ 
	mScreenBuffer = screenbuffer;
}

-(void) DrawPixelsFromBuffer:(unsigned char*)screenBuffer X:(int)x Y:(int)y Width:(int)w Height:(int)h{
	[self setNeedsDisplay:YES];
}


- (void)drawRect:(NSRect)rect
{
	int x;
	int y;
	unsigned char b;

	[[self window] setTitle:@"Bruno's Commodore 64 Emulator"];
	
	NSSize master = rect.size;  
	
	float w = (master.width / (320.0 + 20.0) );
	float h = (master.height / (200.0 + 20.0) );
	float p = w<h?w:h;
//	if(p<1.0)p=1;
	NSRect pixelRect = NSMakeRect(0,0,p,p);


	NSRect myRect = NSMakeRect(0,0,master.width,master.height);
	[m_CBM_COLOR_LIGHT_BLUE_14 set]; //Light Blue
	NSRectFill ( myRect );

	NSRect cbmRect;
	cbmRect.origin.x = (master.width - (320*pixelRect.size.width) )/2.0;
	cbmRect.origin.y = (master.height - (200*pixelRect.size.height))/2.0;
	cbmRect.size.width = 320*pixelRect.size.width;
	cbmRect.size.height = 200*pixelRect.size.height;
	[m_CBM_COLOR_BLUE_6 set]; //Blue
	NSRectFill ( cbmRect );

	
	[m_CBM_COLOR_LIGHT_BLUE_14 set]; //Light Blue

	for(x=0;x<320;x++){
		for(y=0;y<200;y++){
			pixelRect.origin.x = cbmRect.origin.x + (x * pixelRect.size.width);
			pixelRect.origin.y = cbmRect.origin.y + (200*pixelRect.size.height) - (y*pixelRect.size.height);
			b = mScreenBuffer[ (x/8) + (y * 40 /*(320/8)*/ )];
			if((b & (1<< (x%8))) > 0){
				NSRectFill(pixelRect);
			}
		}
	}

}


-(void) DrawCharAt:(int)address character:(char)c{
//	NSString* a = [[NSString alloc] initWithString:@"lala"];
//
//	NSRect myRect = NSMakeRect(10,10,30,30);
//	[[NSColor redColor ] set];
//	NSRectFill ( myRect );
//	[a drawInRect:myRect withAttributes:NULL];
	[self setNeedsDisplay:YES];
	printf("%c", c);
	fflush(stdout);	
}

-(void) DrawCharsFromMemory:(unsigned char*)memory{
	mCharMem = memory;
	[self setNeedsDisplay:YES];
}

- (BOOL)acceptsFirstResponder{
    return YES;
}

- (void)keyDown:(NSEvent*)event{
    NSLog([event characters]);
	char c = (char)[[event characters] characterAtIndex:0];
printf("c=%d\n", c);
	switch(c){
		case 0: //up
			c=145;
			break;
		case 1: //down
			c=17;
			break;
		case 2: //left
			c=157;
			break;
		case 3: //right
			c=29;
			break;
		case 27: //Run stop
			c=127;
			break;
		default:
			if(c >= 'a' && c <= 'z') c=c-32;
			break;
	}
	
//	int x = 0;
//	for(x=0;x<128;x++){
//		if(CVIEWTextMap[x] == c){
//			c=x;
//			break;
//		}
//	}
	
	[mC64 AddKeyStroke:c];
	
}



@end
