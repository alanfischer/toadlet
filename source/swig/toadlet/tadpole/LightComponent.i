%{
#include <toadlet/tadpole/LightComponent.h>
%}

namespace toadlet{
namespace tadpole{

class LightComponent:public Component{
public:
	void destroy();

	void setDirection(float direction[3]);
};

}
}
