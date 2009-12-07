//
//  LogoAppDelegate.m
//  Logo 
//
//  Created by Alan Fischer on 7/10/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "LogoAppDelegate.h"
#include "Logo.h"

@implementation LogoAppDelegate

@synthesize window;

- (void) startApplication:(id) mainWindow{
	if(logo!=NULL){
		return;
	}

	logo=new Logo();
	
	((Application*)logo)->create(mainWindow);
	logo->start(false);
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {    
	[application setStatusBarHidden:YES animated:NO];
	[self startApplication:window];
	[window makeKeyAndVisible];
}


- (void)dealloc {
	logo->destroy();
	delete logo;
    [window release];
    [super dealloc];
}


@end
