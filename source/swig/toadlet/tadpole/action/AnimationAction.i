%{
#include <toadlet/tadpole/action/AnimationAction.h>
%}

namespace toadlet{
namespace tadpole{
namespace action{

class Animation;

class AnimationAction:public Action{
public:
	enum Cycling{
		Cycling_NONE,
		Cycling_LOOP,
		Cycling_REFLECT,
	};

	AnimationAction();
	
	void setTime(int time);
	int getTime() const;

	void setTimeScale(scalar scale);
	scalar getTimeScale() const;

	void setCycling(Cycling cycling);
	Cycling getCycling() const;

	void setStopGently(bool stop);
	
	void attach(Animation *animation);
	void remove(Animation *animation);

	void start();
	void stop();
	
	void update(int dt);
	bool getActive() const;
		
	void addActionListener(ActionListener *listener);
	void removeActionListener(ActionListener *listener);
};

}
}
}
