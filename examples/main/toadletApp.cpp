#include <toadlet/pad.h>

extern Applet *createApplet(Application *app);
extern void destroyApplet(Applet *applet);

int toadletMain(int argc,char **argv){
	PlatformApplication app;
	Applet *applet=createApplet(&app);
	app.setApplet(applet);
	app.create("gl","");
	app.start();
	app.destroy();
	destroyApplet(applet);
	return 1;
}
