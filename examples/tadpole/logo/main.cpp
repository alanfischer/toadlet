#include "Logo.h"

int main(int argc,char **argv){
	Logo app("../../../data/lt.xmsh");
	app.setSize(640,480);
	app.setFullscreen(false);
	app.create();
	app.start(true);
	app.destroy();
	return 0;
}

