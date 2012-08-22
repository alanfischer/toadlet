%{
#include <toadlet/tadpole/ActionComponent.h>
%}

namespace toadlet{
namespace tadpole{

#%feature("director") ActionListener;

class ActionComponent:public Component{
public:
	ActionComponent(String name,Action *action=NULL);
	void destroy();

	void setName(String name);
	String getName() const;

	bool getActive() const;
	
	void start();
	void stop();
	
	void attach(Action *action);
	void remove(Action *action);
};

}
}
