%{
#include <toadlet/tadpole/AnimationActionComponent.h>
%}

namespace toadlet{
namespace tadpole{

class AnimationActionComponent:public Component{
public:
	enum Cycling{
		Cycling_NONE,
		Cycling_LOOP,
		Cycling_REFLECT,
	};

	AnimationActionComponent(String name);
	void destroy();

	void setName(String name);
	String getName() const;

	void setTime(int time);
	int getTime() const;

	void setTimeScale(scalar scale);
	scalar getTimeScale() const;

	void setCycling(Cycling cycling);
	Cycling getCycling() const;

	void start();
	void stop();
	
	void attach(toadlet::tadpole::animation::Animation *animation);
	void remove(toadlet::tadpole::animation::Animation *animation);
};

}
}
