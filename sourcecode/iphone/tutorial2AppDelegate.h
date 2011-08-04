//
//  tutorial2AppDelegate.h
//  tutorial2
//
//  Created by Daniel Jeppsson on 1/6/10.
//  Copyright SouthEnd AB 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EAGLView;

@interface tutorial2AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    EAGLView *glView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;

@end

