%{
#include <toadlet/tadpole/Action.h>
%}

namespace toadlet{
namespace tadpole{

#%feature("director") ActionListener;

class Action;

class ActionListener{
public:
	virtual ~ActionListener(){}

	virtual void actionStarted(Action *action)=0;
	virtual void actionStopped(Action *action)=0;
};

class Action{
public:
	virtual ~Action(){}

	virtual void start()=0;
	virtual void stop()=0;
	virtual void update(int dt)=0;
	virtual bool getActive() const=0;
	
	void addActionListener(ActionListener *listener)=0;
	void removeActionListener(ActionListener *listener)=0;
};

}
}
