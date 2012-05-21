%{
#	include <toadlet/ribbit/AudioBuffer.h>
%}

namespace toadlet{
namespace ribbit{

%refobject AudioBuffer "$this->retain();"
%unrefobject AudioBuffer "$this->release();"

class AudioBuffer{
public:
	virtual ~AudioBuffer();

	virtual bool create(AudioStream *stream)=0;
	virtual void destroy()=0;
};

}
}
