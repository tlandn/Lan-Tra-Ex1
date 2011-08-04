/////////////////////////////////////////////////////////////////
//
// tutorial2AppDelegate.mm
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

#import "tutorial2AppDelegate.h"
#import "EAGLView.h"
#include "djdevicemanager.h"
#include "unistd.h"

@implementation tutorial2AppDelegate

@synthesize window;
@synthesize glView;

- (void) applicationDidFinishLaunching:(UIApplication *)application
{
	sleep(2);
	
	[glView startAnimation];
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0/10)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
}

- (void) applicationWillResignActive:(UIApplication *)application
{
	[glView stopAnimation];
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
	[glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	[glView stopAnimation];
	theDeviceManager.Term();
}

- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	//printf("%.2f %.2f %.2f\n", acceleration.x, acceleration.y, acceleration.z);
	theDeviceManager.OnAccelerate(acceleration.x, acceleration.y, acceleration.z);
} 

- (void) dealloc
{
	[window release];
	[glView release];
	
	[super dealloc];
}

@end
