#include "RibbitTest.h"

RibbitTest::RibbitTest():Application(){
}

RibbitTest::~RibbitTest(){
}

void create(){
	audioBuffer=getAudioPlayer()->createAudioBuffer();

#		if defined(TOADLET_PLATFORM_WINCE)
		String filename="\\Program Files\\ribbitTest\\constructo_putty_hit.wav";
//			String filename="\\Program Files\\ribbitTest\\lawndarts_relaxed.mid";
#		else
		String filename="../../data/constructo_putty_hit.wav";
#		endif
	FileInputStream::Ptr in(new FileInputStream(filename));
	audioBuffer=AudioBuffer::Ptr(new AudioBuffer(audioPlayer,in,filename.substr(filename.rfind('.')+1,3)));
	audio=Audio::Ptr(new Audio(audioPlayer));
	audio->loadAudioBuffer(audioBuffer);
	audio->setLooping(true);
	audio->play();
}

void destroy(){
	audio->destroy();
	audioBuffer->destroy();
}

