#include "FindTest.h"
#include <toadlet/egg/System.h>

using namespace toadlet::egg;

FindTest::FindTest():Application(){
	connector=LANPeerEventConnector::ptr(new LANPeerEventConnector());
}

FindTest::~FindTest(){
}

void FindTest::create(int port){
	Application::create();

	connector->create(true,12345,port,"FindTest",1,NULL);

	if(connector->search(NULL)==false){
		Logger::alert("Problem");
		return;
	}
}

void FindTest::destroy(){
	connector->close();

	Application::destroy();
}

void FindTest::update(int dt){
	int result=connector->update();

	Logger::alert(String("result:")+result+" order:"+connector->getOrder()+" seed:"+connector->getSeed());

	System::msleep(100);
}

#if defined(TOADLET_PLATFORM_WINCE)
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
#else
int main(int argc,char **argv){
#endif
	if(argc<2){return 0;}

	FindTest app;
	app.setFullscreen(false);
	app.create(String(argv[1]).toInt32());
	app.start(true);
	app.destroy();
	return 0;
}
