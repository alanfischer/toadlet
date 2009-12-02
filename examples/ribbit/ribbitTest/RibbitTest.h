#ifndef RIBBITTEST_H
#define RIBBITTEST_H

#include <toadlet/pad/Application.h>

using namespace toadlet::egg;
using namespace toadlet::ribbit;

class RibbitTest:public Application{
public:
	RibbitTest();
	virtual ~RibbitTest();
	
	void create();
	void destroy();

protected:
	AudioBuffer::ptr audioBuffer;
	Audio::ptr audio;
};

#endif
