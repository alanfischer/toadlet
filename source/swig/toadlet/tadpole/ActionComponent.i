%{
#include <toadlet/tadpole/ActionComponent.h>
%}

namespace toadlet{
namespace tadpole{

#%feature("director") ActionListener;

class ActionComponent:public Component{
public:
	ActionComponent(char *name);
	void destroy();

	void setName(char *name);

	bool getActive() const;
	
	void start();
	void stop();
	
	void attach(Action *action);
	void remove(Action *action);
};

}
}
