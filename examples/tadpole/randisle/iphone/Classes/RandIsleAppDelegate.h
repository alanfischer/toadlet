#import <UIKit/UIKit.h>

class RandIsle;

@interface RandIsleAppDelegate : NSObject <UIApplicationDelegate> {
	UIWindow *window;
	RandIsle *app;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

