%{
#	include <toadlet/ribbit/BaseAudioDevice.h>
%}

namespace toadlet{
namespace ribbit{

#%feature("director") BaseAudioDevice;

class BaseAudioDevice:public AudioDevice{
public:
	virtual bool create();
	virtual void destroy();

	virtual AudioBuffer *createAudioBuffer();
	virtual Audio *createAudio();
	
	virtual void update(int dt);
};

}
}
