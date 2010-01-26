#if 0

#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/knot/LANPeerEventConnector.h>
#include "../quicktest.h"

using namespace toadlet::egg;
using namespace toadlet::knot;

QT_TEST(LANPeerEventConnectorTestTCP){
	LANPeerEventConnector::ptr connector1(new LANPeerEventConnector());
	LANPeerEventConnector::ptr connector2(new LANPeerEventConnector());

	QT_CHECK(connector1->create(false,6968,6969,"LANPeerEventConnectorTest",1,NULL));
	QT_CHECK(connector2->create(false,6968,6970,"LANPeerEventConnectorTest",1,NULL));

	Random random(System::mtime());
	QT_CHECK(connector1->search(random.nextInt(),NULL));
	System::msleep(100);

	QT_CHECK(connector2->search(random.nextInt(),NULL));
	System::msleep(100);

	int result1=0,result2=0;
	int i;
	for(i=0;i<10 && (result1<=0 || result2<=0);++i){
		if(result1<=0) result1=connector1->update();
		if(result2<=0) result2=connector2->update();
		System::msleep(100);
	}

	QT_CHECK(result1==1 && result2==1);
	QT_CHECK_NOT_EQUAL(connector1->getOrder(),connector2->getOrder());
	QT_CHECK_EQUAL(connector1->getSeed(),connector2->getSeed());

	connector1->close();
	connector2->close();
}

QT_TEST(LANPeerEventConnectorTestUDP){
	LANPeerEventConnector::ptr connector1(new LANPeerEventConnector());
	LANPeerEventConnector::ptr connector2(new LANPeerEventConnector());

	QT_CHECK(connector1->create(true,6971,6972,"LANPeerEventConnectorTest",1,NULL));
	QT_CHECK(connector2->create(true,6971,6973,"LANPeerEventConnectorTest",1,NULL));

	Random random(System::mtime());
	QT_CHECK(connector1->search(random.nextInt(),NULL));
	System::msleep(100);

	QT_CHECK(connector2->search(random.nextInt(),NULL));
	System::msleep(100);

	int result1=0,result2=0;
	int i;
	for(i=0;i<10 && (result1<=0 || result2<=0);++i){
		if(result1<=0) result1=connector1->update();
		if(result2<=0) result2=connector2->update();
		System::msleep(100);
	}

	QT_CHECK(result1==1 && result2==1);
	QT_CHECK_NOT_EQUAL(connector1->getOrder(),connector2->getOrder());
	QT_CHECK_EQUAL(connector1->getSeed(),connector2->getSeed());

	connector1->close();
	connector2->close();
}

#endif