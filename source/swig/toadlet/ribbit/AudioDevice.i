%{
#	include <toadlet/ribbit/AudioDevice.h>
%}

namespace toadlet{
namespace ribbit{

%refobject AudioDevice "$this->retain();"
%unrefobject AudioDevice "$this->release();"

SWIG_JAVABODY_PROXY(public, public, AudioDevice)

class AudioDevice{
public:
	virtual bool create()=0;
	virtual void destroy()=0;

	virtual AudioBuffer *createAudioBuffer()=0;
	virtual Audio *createAudio()=0;
	
	virtual void update(int dt)=0;
};

}
}
