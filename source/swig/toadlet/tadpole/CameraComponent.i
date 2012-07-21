%{
#include <toadlet/tadpole/CameraComponent.h>
%}

namespace toadlet{
namespace tadpole{

class CameraComponent:public Component{
public:
	void destroy();

	void setClearColor(float color[4]);

	void setLookAt(float eye[3],float point[3],float up[3]);

	void setAutoProjectionFov(float fov,float nearDist,float farDist);
};

}
}
