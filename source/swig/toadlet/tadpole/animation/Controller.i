%{
#include <toadlet/tadpole/animation/Controller.h>
%}

namespace toadlet{
namespace tadpole{
namespace animation{

%feature("director") ControllerListener;

class ControllerListener{
public:
	virtual void controllerFinished(toadlet::tadpole::animation::Controller *controller)=0;
};

class Controller:public Component{
public:
	enum Cycling{
		Cycling_NONE,
		Cycling_LOOP,
		Cycling_REFLECT,
	};

	void destroy();

	void setName(String name);
	String getName() const;

	void setTime(int time);
	int getTime() const;

	void setTimeScale(scalar scale);
	scalar getTimeScale() const;

	void setCycling(Cycling cycling);
	Cycling getCycling() const;

	void setControllerListener(ControllerListener *listener);

	void start();
	void stop();
	
	void attach(Animation *animation);
	void remove(Animation *animation);
};

}
}
}
