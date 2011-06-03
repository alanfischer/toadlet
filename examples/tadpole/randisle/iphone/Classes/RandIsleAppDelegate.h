//
//  ShovelStuffAppDelegate.h
//  ShovelStuff
//
//  Created by Alan Fischer on 7/10/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

class Logo;

@interface LogoAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	Logo *logo;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

