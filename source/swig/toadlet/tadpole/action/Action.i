%{
#include <toadlet/tadpole/action/Action.h>
%}

namespace toadlet{
namespace tadpole{
namespace action{

class ActionListener;

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
}
