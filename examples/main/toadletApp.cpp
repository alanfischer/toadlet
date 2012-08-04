#include <toadlet/pad.h>

extern Applet *createApplet(Application *app);
extern void destroyApplet(Applet *applet);

int toadletMain(int argc,char **argv){
	PlatformApplication app;
	Applet::ptr applet=createApplet(&app);
	app.setApplet(applet);
	app.defaultCreate();
	app.start();
	app.destroy();
	return 1;
}
