/////////////////////////////////////////////////////////////////
//
// EAGLView.mm
// GL view
//
/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Daniel Jeppsson
// All Rights Reserved. These instructions, statements, computer
// programs, and/or related material (collectively, the "Source")
// contain unpublished information propietary to Daniel Jeppsson
// which is protected by US federal copyright law and by
// international treaties. This Source may NOT be disclosed to
// third parties, or be copied or duplicated, in whole or in
// part, without the written consent of Daniel Jeppsson.
//
/////////////////////////////////////////////////////////////////
//
// Author: Daniel Jeppsson
//
/////////////////////////////////////////////////////////////////

#import "EAGLView.h"

#import "ES1Renderer.h"
#import "ES2Renderer.h"

#include "djdevicemanager.h"
#include "djengine.h"
#include "djfile.h"

UITouch* g_touchTracking[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

int GetTouchID(UITouch *ti, bool bRemove)
{
	//DJInfo("%d", (int)ti);
	int nAvailable = -1;
	for (int q=0; q<10; q++)
	{
		if (g_touchTracking[q] == ti)
		{
			if (bRemove)
				g_touchTracking[q] = NULL;
			return q;
		}
		else if (g_touchTracking[q] == NULL && nAvailable == -1)
		{
			nAvailable = q;
		}
	}
	
	if (nAvailable != -1)
	{
		if (!bRemove)
		{
			g_touchTracking[nAvailable] = ti;
		}
		return nAvailable;
	}

	DJWarning("Failed to find available slot for tracking touch!");
		
	return 0;
}

@implementation EAGLView

@synthesize animating;
@dynamic animationFrameInterval;

// You must implement this method
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id) initWithCoder:(NSCoder*)coder
{    
	DJTrace("%s()", __FUNCTION__);
	
	CGRect size = self.bounds;
	theDeviceManager.OnResize(size.size.width, size.size.height);	
	
    if ((self = [super initWithCoder:coder]))
	{
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		renderer = [[ES2Renderer alloc] init];
		
		if (!renderer)
		{
			renderer = [[ES1Renderer alloc] init];
			
			if (!renderer)
			{
				[self release];
				return nil;
			}
		}
		
		int nErr = glGetError();
        
		animating = FALSE;
		displayLinkSupported = FALSE;
		animationFrameInterval = 1;
		displayLink = nil;
		animationTimer = nil;
		
		// A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
		// class is used as fallback when it isn't available.
		NSString *reqSysVer = @"3.1";
		NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
		if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
			displayLinkSupported = TRUE;
		
    }
	
	//CFStringRef homeDir = (CFStringRef)NSHomeDirectory();
	self.multipleTouchEnabled = YES;
	
    return self;
}

- (void) init
{
	DJTrace("%s()", __FUNCTION__);
	char path[256];
	//sprintf( path , "%s/",[homeDir UTF8String] );
	//printf("\n\nPATH: '%s'\n\n", path);
	
	NSString *myFilePath = [[NSBundle mainBundle]
							pathForResource:@"data"
							ofType:@""];
	
	sprintf( path , "%s/",[myFilePath UTF8String] );
	printf("\n\nPATH: '%s'\n\n", path);
	
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
	
	theDeviceManager.Init(path);
	
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
	
}

- (void) drawView:(id)sender
{
	theDeviceManager.Update();
	
    [renderer renderBegin];
	theDeviceManager.Paint();	
    [renderer renderEnd];
}

- (void) layoutSubviews
{
	//int a =self.bounds.size.width;
	//a = self.bounds.size.height;
	//a = self.window.screen.bounds.size.width;
	//a = self.window.screen.bounds.size.height;
	[renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
	
	//CGRect size = bounds;
	//theDeviceManager.OnResize(size.width, size.height);
	static bool bFirst = DJTRUE;
	if (bFirst)
	{
		[self init];
	}
	
    [self drawView:nil];
}

- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	// Frame interval defines how many display frames must pass between each time the
	// display link fires. The display link will only fire 30 times a second when the
	// frame internal is two on a display that refreshes 60 times a second. The default
	// frame interval setting of one will fire 60 times a second when the display refreshes
	// at 60 times a second. A frame interval setting of less than one results in undefined
	// behavior.
	if (frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if (animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void) startAnimation
{
	if (!animating)
	{
		if (displayLinkSupported)
		{
			// CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
			// if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
			// not be called in system versions earlier than 3.1.

			displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
			[displayLink setFrameInterval:animationFrameInterval];
			[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		}
		else
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];
		
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if (animating)
	{
		if (displayLinkSupported)
		{
			[displayLink invalidate];
			displayLink = nil;
		}
		else
		{
			[animationTimer invalidate];
			animationTimer = nil;
		}
		
		animating = FALSE;
	}
}

- (void) dealloc
{
    [renderer release];
	
	theDeviceManager.Term();
	
    [super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	//theDeviceManager.OnTouchBegin(event->);
	//printf("touchesBegan\n");
    //NSArray *allTouches = [touches allObjects];
    //UITouch *touch = [touches anyObject];
	//CGPoint touchPt;
    
    //int count = [allTouches count];
    for (UITouch *myTouch in touches)
	{
        CGPoint touchPt = [myTouch locationInView:self];
        //NSTimeInterval myTouchID = [myTouch timestamp ];
		int nID = GetTouchID(myTouch, false);
		
		// Turn into landscape coordinates
		float temp = touchPt.x;
		touchPt.x = touchPt.y;
		touchPt.y = 320-temp;
		theDeviceManager.OnTouchBegin(nID, touchPt.x, touchPt.y);
	}
    
	/*
    if (count >= 1) 
	{
		touchPt = [[allTouches objectAtIndex:0] locationInView:self];
		//printf("%.2f %.2f\n", touchPt.x, touchPt.y);
		theDeviceManager.OnTouchBegin(touchPt.x, touchPt.y);
	 }
	 */
    
	
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSArray *allTouches = [touches allObjects];
    
    //int count = [allTouches count];
    
    for (UITouch *myTouch in touches)
	{
        CGPoint touchPt = [myTouch locationInView:self];
		// Turn into landscape coordinates
		float temp = touchPt.x;
		touchPt.x = touchPt.y;
		touchPt.y = 320-temp;

        //NSTimeInterval myTouchID = [myTouch timestamp ];
		int nID = GetTouchID(myTouch, false);
		theDeviceManager.OnTouchMove(nID, touchPt.x, touchPt.y);
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    //NSArray *allTouches = [touches allObjects];
    //UITouch *touch = [touches anyObject];
	//CGPoint touchPt;
    
    //int count = [allTouches count];
    
    for (UITouch *myTouch in touches)
	{
        CGPoint touchPt = [myTouch locationInView:self];
		// Turn into landscape coordinates
		float temp = touchPt.x;
		touchPt.x = touchPt.y;
		touchPt.y = 320-temp;
		
        //NSTimeInterval myTouchID = [myTouch timestamp ];
		int nID = GetTouchID(myTouch, true);
		theDeviceManager.OnTouchEnd(nID, touchPt.x, touchPt.y);
	}
}

@end
