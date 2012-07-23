%{
#include <toadlet/tadpole/CameraComponent.h>
%}

namespace toadlet{
namespace tadpole{

class CameraComponent:public Component{
public:
	void destroy();

	Camera *getCamera();
};

}
}
