%{
#	include <toadlet/ribbit/BaseAudioBuffer.h>
%}

namespace toadlet{
namespace ribbit{

#%feature("director") BaseAudioBuffer;

class BaseAudioBuffer:public AudioBuffer{
public:
	virtual ~BaseAudioBuffer();

	virtual bool create(AudioStream *stream);
	virtual void destroy();
};

}
}
