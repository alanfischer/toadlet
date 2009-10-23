#include <toadlet/ribbit/AudioPlayer.h>
#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/Logger.h>

#if defined(TOADLET_PLATFORM_WINCE)
#	define USE_WINCEPLAYER
#else
#	define USE_ALPLAYER
//#	define USE_AUDIEREPLAYER
#endif
#define TOADLET_USE_STATIC
#define USE_LINK_RIBBIT_PLUGIN

#include <toadlet/Symbols.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::ribbit;

class RibbitTest{
public:
	RibbitTest(){
		audioPlayer=new_AudioPlayer();

		bool result=audioPlayer->startup();
		if(result==false){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
				"Error starting audio player");
			delete audioPlayer;
			audioPlayer=NULL;
			return;
		}

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

	~RibbitTest(){
		audio=Audio::Ptr();
		audioBuffer=AudioBuffer::Ptr();

		if(audioPlayer!=NULL){
			audioPlayer->shutdown();
			delete audioPlayer;
			audioPlayer=NULL;
		}
	}

	void run(){
		TOADLET_LOG(NULL,Logger::LEVEL_ALERT,
			"Playing a sound for 5 seconds");
		System::msleep(5000);
	}

	AudioPlayer *audioPlayer;
	AudioBuffer::Ptr audioBuffer;
	Audio::Ptr audio;
};

int uniformMain(int argc,char **argv){
	RibbitTest test;
	if(test.audioPlayer!=NULL){
		test.run();
	}
	return 0;
}
