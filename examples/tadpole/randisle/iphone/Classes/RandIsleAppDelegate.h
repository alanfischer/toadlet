#import <UIKit/UIKit.h>
#include <toadlet/pad.h>

@interface RandIsleAppDelegate : NSObject <UIApplicationDelegate> {
	UIWindow *window;
	PlatformApplication *app;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

