%{
#include <toadlet/tadpole/Camera.h>
%}

namespace toadlet{
namespace tadpole{

class Camera{
public:
	Camera(Engine *engine);

	void setClearColor(float color[4]);

	void setLookAt(float eye[3],float point[3],float up[3]);

	void setAutoProjectionFov(float fov,bool yheight,float nearDist,float farDist);

	void render(toadlet::peeper::RenderDevice *device,Scene *scene);
};

}
}
