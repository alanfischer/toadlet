%{
#	include <toadlet/ribbit/BaseAudio.h>
%}

namespace toadlet{
namespace ribbit{

#%feature("director") BaseAudio;

class BaseAudio:public Audio{
public:
	virtual ~BaseAudio();

	virtual bool create(AudioBuffer *buffer);
	virtual bool create(AudioStream *stream);
	virtual void destroy();

	virtual AudioBuffer *getAudioBuffer();
	virtual AudioStream *getAudioStream();

	virtual bool play();
	virtual bool stop();
	virtual bool getPlaying() const;
	virtual bool getFinished() const;

	virtual void setLooping(bool looping);
	virtual bool getLooping() const;

	virtual void setGain(scalar gain);
	virtual scalar getGain() const;

	virtual void setPitch(scalar pitch);
	virtual scalar getPitch() const;

	virtual void setGlobal(bool global);
	virtual bool getGlobal() const;

	virtual void setPosition(float position[3]);
	virtual void setVelocity(float velocity[3]);

	virtual void setRolloffFactor(scalar f);
	virtual scalar getRolloffFactor() const;
};

}
}
