#include "RibbitTest.h"
#include "boing_wav.h"

// To keep this example as simple as possible, it does not require any other data files, instead getting its data from lt_xmsh
RibbitTest::RibbitTest():Application(){
}

RibbitTest::~RibbitTest(){
}

void RibbitTest::create(){
	Application::create();

	AudioStream::ptr stream(new WaveDecoder());
	if(stream->startStream(MemoryStream::ptr(new MemoryStream(boing_wav::data,boing_wav::length,boing_wav::length,false)))==false){
		Error::unknown("unable to start stream");
		return;
	}

	audioBuffer=AudioBuffer::ptr(getAudioPlayer()->createAudioBuffer());
	audioBuffer->create(stream);

	audio=Audio::ptr(getAudioPlayer()->createBufferedAudio());
	audio->create(audioBuffer);
	audio->play();
}

void RibbitTest::destroy(){
	audio->destroy();
	audioBuffer->destroy();

	Application::destroy();
}

void RibbitTest::update(int dt){
	if(audio->getPlaying()==false){
		stop();
	}
}

int toadletMain(int argc,char **argv){
	RibbitTest app;
	app.create();
	app.start();
	app.destroy();
	return 0;
}
