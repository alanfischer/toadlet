#ifndef FINDTEST_H
#define FINDTEST_H

#include <toadlet/knot/LANPeerEventConnector.h>
#include <toadlet/pad/Application.h>

using namespace toadlet::knot;
using namespace toadlet::pad;

class FindTest:public Application{
public:
	FindTest();
	virtual ~FindTest();

	void create(int port);
	void destroy();

	void update(int dt);

protected:
	LANPeerEventConnector::ptr connector;
};

#endif
