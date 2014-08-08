#include <toadlet/pad.h>

using namespace toadlet::pad;

extern Applet *createApplet(Application *app);

int toadletMain(int argc,char **argv){
	PlatformApplication::ptr app=new PlatformApplication();
	Applet::ptr applet=createApplet(app);
	app->setApplet(applet);
	app->setSize(400,400);
	app->create("d3d9","mm");
	app->start();
	app->destroy();
	return 1;
}
