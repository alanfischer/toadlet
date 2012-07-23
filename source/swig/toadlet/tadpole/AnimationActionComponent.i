%{
#include <toadlet/tadpole/AnimationActionComponent.h>
%}

namespace toadlet{
namespace tadpole{

#%feature("director") ActionListener;

class AnimationActionComponent;

class ActionListener{
public:
	virtual ~ActionListener(){}

	virtual void actionStarted(AnimationActionComponent *component)=0;
	virtual void actionStopped(AnimationActionComponent *component)=0;
};

class AnimationActionComponent:public Component{
public:
	enum Cycling{
		Cycling_NONE,
		Cycling_LOOP,
		Cycling_REFLECT,
	};

	AnimationActionComponent(char *name);
	void destroy();

	void setName(char *name);

	bool getActive() const;
	
	void setTime(int time);
	int getTime() const;

	void setTimeScale(scalar scale);
	scalar getTimeScale() const;

	void setCycling(Cycling cycling);
	Cycling getCycling() const;

	void setStopGently(bool stop);
	
	void start();
	void stop();
	
	void attach(toadlet::tadpole::animation::Animation *animation);
	void remove(toadlet::tadpole::animation::Animation *animation);
	
	void setActionListener(ActionListener *listener);
	ActionListener *getActionListener() const;
};

}
}
