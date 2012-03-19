%{
#include <toadlet/tadpole/animation/Controller.h>
%}

namespace toadlet{
namespace tadpole{
namespace animation{

class Controller:public Component{
public:
	Controller();

	void setTime(int time);
	int getTime();

	void setTimeScale(float scale);
	float getTimeScale();

	void start();
	void stop();
	
	void attach(Animation *animation);
	void remove(Animation *animation);
};

}
}
}
