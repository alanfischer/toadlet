#include <toadlet/pad.h>

extern Applet *createApplet(Application *app);

#if defined(TOADLET_PLATFORM_ANDROID)
#else
int toadletMain(int argc,char **argv){
	Application app;
	Applet *applet=createApplet(&app);
	app.setApplet(applet);
	app.create();
	app.start();
	app.destroy();
	delete applet;
	return 1;
}
#endif
