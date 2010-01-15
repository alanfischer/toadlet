#include <toadlet/egg/System.h>
#include <toadlet/knot/LANPeerEventConnector.h>
#include "../quicktest.h"

QT_TEST(LANPeerEventConnectorTest){
	LANPeerEventConnector connector(new LANPeerEventConnector());

	QT_CHECK(connector->create(true,12345,port,"LANPeerEventConnectorTest",1,NULL));

	QT_CHECK(connector->search(NULL));
	
	int i;
	for(i=0;i<10;++i){
		if(connector->update()>=0) break;
		System::msleep(100);
	}
	QT_CHECK(i!=10);
	QT_CHECK(connector->getOrder()==0 || i!=10);

	Logger::alert(String("result:")+result+" order:"+connector->getOrder()+" seed:"+connector->getSeed());

}
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
