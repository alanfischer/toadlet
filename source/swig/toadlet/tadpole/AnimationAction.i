%{
#include <toadlet/tadpole/AnimationAction.h>
%}

namespace toadlet{
namespace tadpole{

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
	
	void attach(toadlet::tadpole::animation::Animation *animation);
	void remove(toadlet::tadpole::animation::Animation *animation);

	void start();
	void stop();
	
	void update(int dt);
	bool getActive() const;
		
	void addActionListener(ActionListener *listener);
	void removeActionListener(ActionListener *listener);
};

}
}
