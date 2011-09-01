#import <UIKit/UIKit.h>

class Logo;

@interface LogoAppDelegate : NSObject <UIApplicationDelegate> {
	UIWindow *window;
	Logo *app;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

