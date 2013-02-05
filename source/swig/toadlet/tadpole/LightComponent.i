%{
#include <toadlet/tadpole/LightComponent.h>
%}

namespace toadlet{
namespace tadpole{

class LightComponent:public Component{
public:
	void destroy();

	virtual void setName(String name);
	virtual String getName() const;
	
	virtual bool getActive() const;

	void setDirection(float direction[3]);
};

}
}
