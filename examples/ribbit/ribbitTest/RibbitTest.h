#ifndef RIBBITTEST_H
#define RIBBITTEST_H

#include <toadlet/egg/io/MemoryInputStream.h>
#include <toadlet/pad/Application.h>

using namespace toadlet::egg::io;
using namespace toadlet::ribbit;
using namespace toadlet::pad;

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
