#ifndef RIBBITTEST_H
#define RIBBITTEST_H

#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/ribbit/decoder/WaveDecoder.h>
#include <toadlet/pad/Application.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::ribbit;
using namespace toadlet::ribbit::decoder;
using namespace toadlet::pad;

class RibbitTest:public Application{
public:
	RibbitTest();
	virtual ~RibbitTest();
	
	void create();
	void destroy();

	void update(int dt);

protected:
	AudioBuffer::ptr audioBuffer;
	Audio::ptr audio;
};

#endif
