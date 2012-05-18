%{
#	include <toadlet/ribbit/AudioStream.h>
%}
 
namespace toadlet{
namespace ribbit{

%refobject AudioStream "$this->retain();"
%unrefobject AudioStream "$this->release();"

class AudioStream{
public:
	virtual AudioFormat *getAudioFormat() const=0;
	
	virtual void close()=0;
	
	virtual int length()=0;
	virtual void reset()=0;
	virtual int read(tbyte buffer[],int length)=0;
};

}
}
