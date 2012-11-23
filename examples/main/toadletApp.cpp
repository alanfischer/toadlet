#include <toadlet/pad.h>

extern Applet *createApplet(Application *app);
extern void destroyApplet(Applet *applet);

int toadletMain(int argc,char **argv){
	PlatformApplication app;
	Applet *applet=createApplet(&app);
	app.setApplet(applet);
	app.setSize(400,400);
	app.create("gl","mm");
	app.start();
	app.destroy();
	return 1;
}
