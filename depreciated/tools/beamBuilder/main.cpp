#include "BeamBuilder.h"

int main(int argc,char **argv){
	BeamBuilder app;
	app.create(argc,argv);
	app.start(true);
	app.destroy();
}
