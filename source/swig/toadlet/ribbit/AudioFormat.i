%{
#	include <toadlet/ribbit/AudioFormat.h>
%}

namespace toadlet{
namespace ribbit{

class AudioFormat{
public:
	AudioFormat(int bps=0,int chan=0,int sps=0){}

	void setBitsPerSample(int bps);
	int getBitsPerSample();
	
	void setChannels(int channels);
	inline int getChannels();
	
	void setSamplesPerSecond(int sps);
	int getSamplesPerSecond();
	
	int getFrameSize() const;
};

}
}
