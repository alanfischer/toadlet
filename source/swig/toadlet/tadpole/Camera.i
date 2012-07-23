%{
#include <toadlet/tadpole/Camera.h>
%}

namespace toadlet{
namespace tadpole{

class Camera{
public:
	void setClearColor(float color[4]);

	void setLookAt(float eye[3],float point[3],float up[3]);

	void setAutoProjectionFov(float fov,float nearDist,float farDist);
};

}
}
