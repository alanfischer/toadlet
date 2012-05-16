%{
#include <toadlet/tadpole/ActionComponent.h>
%}

namespace toadlet{
namespace tadpole{

class ActionComponent:public Component{
public:
	virtual void start()=0;
	virtual void stop()=0;
};

}
}
