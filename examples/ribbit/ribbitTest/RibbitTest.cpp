#include "RibbitTest.h"
#include "boing_wav.h"

// To keep this example as simple as possible, it does not require any other data files, instead getting its data from lt_xmsh
RibbitTest::RibbitTest():Application(){
}

RibbitTest::~RibbitTest(){
}

void RibbitTest::create(){
	Application::create();

	MemoryInputStream::ptr in(new MemoryInputStream(boing_wav::data,boing_wav::length));

	audioBuffer=AudioBuffer::ptr(getAudioPlayer()->createAudioBuffer());
	audioBuffer->create(in,"audio/x-wav");

	audio=Audio::ptr(getAudioPlayer()->createBufferedAudio());
	audio->create(audioBuffer);
	audio->play();
}

void RibbitTest::destroy(){
	audio->destroy();
	audioBuffer->destroy();

	Application::destroy();
}

#if defined(TOADLET_PLATFORM_WINCE)
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
#else
int main(int argc,char **argv){
#endif
	RibbitTest app;
	app.setFullscreen(false);
	app.create();
	app.start(true);
	app.destroy();
	return 0;
}
