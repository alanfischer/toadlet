//
//  LogoAppDelegate.mm
//  logo
//
//  Created by Alan Fischer on 10/28/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LogoAppDelegate.h"
#include "Logo.h"

@implementation LogoAppDelegate

- (id) init{
	if(self=[super init]){
		self->logo=NULL;
	}
	return self;
}

- (void) startApplication:(id) mainWindow{
	if(logo!=NULL){
		return;
	}
	
	logo=new Logo();
	logo->setWindow(mainWindow);
	logo->create();
	logo->start();
}


- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication*) app{
	return YES;
}

- (NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication*) app{
	return YES;
}

- (void)windowDidBecomeMain:(NSNotification*) notification{
	[self startApplication:[notification object]];
}

@end
