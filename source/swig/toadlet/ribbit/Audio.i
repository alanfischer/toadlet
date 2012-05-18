%{
#	include <toadlet/ribbit/Audio.h>
%}

namespace toadlet{
namespace ribbit{

%refobject Audio "$this->retain();"
%unrefobject Audio "$this->release();"

class Audio{
public:
	virtual ~Audio();

	virtual bool create(AudioBuffer *buffer)=0;
	virtual bool create(AudioStream *stream)=0;
	virtual void destroy()=0;

	virtual AudioBuffer *getAudioBuffer()=0;
	virtual AudioStream *getAudioStream()=0;

	virtual bool play()=0;
	virtual bool stop()=0;
	virtual bool getPlaying() const=0;
	virtual bool getFinished() const=0;

	virtual void setLooping(bool looping)=0;
	virtual bool getLooping() const=0;

	virtual void setGain(scalar gain)=0;
	virtual scalar getGain() const=0;

	virtual void setPitch(scalar pitch)=0;
	virtual scalar getPitch() const=0;

	virtual void setGlobal(bool global)=0;
	virtual bool getGlobal() const=0;

	virtual void setPosition(float position[3])=0;
	virtual void setVelocity(float velocity[3])=0;

	virtual void setRolloffFactor(scalar f)=0;
	virtual scalar getRolloffFactor() const=0;
};

}
}
