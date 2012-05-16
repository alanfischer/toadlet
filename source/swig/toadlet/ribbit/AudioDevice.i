%{
#	include <toadlet/ribbit/AudioDevice.h>
%}

namespace toadlet{
namespace ribbit{

%refobject AudioDevice "$this->retain();"
%unrefobject AudioDevice "$this->release();"

class AudioDevice{
public:
	virtual bool create(int *options)=0;
	virtual void destroy()=0;

	virtual void update(int dt)=0;
};

}
}
/*
%inline %{
extern "C" toadlet::ribbit::AudioDevice *new_ALAudioDevice();
extern "C" toadlet::ribbit::AudioDevice *new_MMAudioDevice();
%}
*/