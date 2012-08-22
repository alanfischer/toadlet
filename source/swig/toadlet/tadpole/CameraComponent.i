%{
#include <toadlet/tadpole/CameraComponent.h>
%}

namespace toadlet{
namespace tadpole{

class CameraComponent:public Component{
public:
	CameraComponent(Camera *camera);
	void destroy();

	Camera *getCamera();
};

}
}
