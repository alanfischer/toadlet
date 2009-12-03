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
	if(shovelstuff!=NULL){
		return;
	}
	
	NSString *string=[[NSBundle mainBundle] pathForResource:@"shovelstuff" ofType:@"tpkg"];
	if(string!=nil){
		String path=[string cStringUsingEncoding:NSASCIIStringEncoding];
		path="-p"+path.substr(0,path.rfind('/'));
		int argc=2;
		char *argv[2]={
			NULL,
			(char*)path.c_str(),
		};
		shovelstuff=new Logo(argc,argv);
	}
	else{
		shovelstuff=new Logo(0,NULL);
	}
	
	((Application*)shovelstuff)->create(mainWindow);
	shovelstuff->start(false);
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {    
	[application setStatusBarHidden:YES animated:NO];
	[self startApplication:window];
	[window makeKeyAndVisible];
}


- (void)dealloc {
	shovelstuff->destroy();
	delete shovelstuff;
    [window release];
    [super dealloc];
}


@end
