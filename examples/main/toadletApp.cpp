#include <toadlet/pad.h>

extern Applet *createApplet(Application *app);

int toadletMain(int argc,char **argv){
	PlatformApplication::ptr app=new PlatformApplication();
	Applet::ptr applet=createApplet(app);
	app->setApplet(applet);
	app->setSize(400,400);
	app->create("null","mm");
	app->start();
	app->destroy();
	return 1;
}
