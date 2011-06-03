#import "RandIsleAppDelegate.h"
#include "RandIsle.h"

@implementation RandIsleAppDelegate

@synthesize window;

- (void) startApplication:(id) mainWindow{
	if(app!=NULL){
		return;
	}

	app=new RandIsle();
	
	app->setWindow(mainWindow);
	app->create("");
	app->start();
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {    
	[application setStatusBarHidden:YES animated:NO];
	[self startApplication:window];
	[window makeKeyAndVisible];
}


- (void)dealloc {
	app->destroy();
	delete app;
    [window release];
    [super dealloc];
}


@end
