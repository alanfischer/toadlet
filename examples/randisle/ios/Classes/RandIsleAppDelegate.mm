#import "RandIsleAppDelegate.h"
#include "RandIsle.h"

@implementation RandIsleAppDelegate

@synthesize window;

- (void) startApplication:(id) mainWindow{
	if(app!=NULL){
		return;
	}

	app=new PlatformApplication();
	app->setWindow(mainWindow);
	app->setApplet(new RandIsle(app,""));
	app->defaultCreate();
	app->start();
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {    
	[application setStatusBarHidden:YES animated:NO];
	[self startApplication:window];
	[window makeKeyAndVisible];
}

- (void)applciationDidEnterBackground:(UIApplication *)application {
	app->stop();
}

- (void)applciationDidEnterForeground:(UIApplication *)application {
	app->start();
}

- (void)dealloc {
	app->destroy();
	delete app;
    [window release];
    [super dealloc];
}


@end
