//
//  tadpoleTestAppDelegate.m
//  tadpoleTest
//
//  Created by Alan Fischer on 9/26/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "tadpoleTestAppDelegate.h"
#include "../../main.cpp"

@implementation tadpoleTestAppDelegate

@synthesize window;

TadpoleTest *test=NULL;

- (void)applicationDidFinishLaunching:(UIApplication *)application {	
	test=new TadpoleTest();
	test->run();
}


- (void)dealloc {
	[window release];
	[super dealloc];
}


@end
