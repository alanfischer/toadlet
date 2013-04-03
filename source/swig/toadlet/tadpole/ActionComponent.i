%{
#include <toadlet/tadpole/ActionComponent.h>
%}

namespace toadlet{
namespace tadpole{

#%feature("director") ActionListener;

class ActionComponent:public Component{
public:
	ActionComponent(String name,toadlet::tadpole::action::Action *action=NULL);
	void destroy();

	void setName(String name);
	String getName() const;

	bool getActive() const;
	
	void start();
	void stop();
};

}
}
