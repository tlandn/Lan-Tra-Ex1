//
//  ESRenderer.h
//  tutorial2
//
//  Created by Daniel Jeppsson on 1/6/10.
//  Copyright SouthEnd AB 2010. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

@protocol ESRenderer <NSObject>

- (void) renderBegin;
- (void) renderEnd;
- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer;

@end
