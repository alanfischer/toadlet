#include <toadlet/pad.h>

extern Applet *createApplet(Application *app);
extern void destroyApplet(Applet *applet);

#if defined(TOADLET_PLATFORM_ANDROID)
#else
int toadletMain(int argc,char **argv){
	Application app;
	Applet *applet=createApplet(&app);
	app.setApplet(applet);
	app.create();
	app.start();
	app.destroy();
	destroyApplet(applet);
	return 1;
}
#endif
