%{
#include <toadlet/tadpole/sg/CameraEntity.h>
%}

namespace toadlet{
namespace tadpole{
namespace sg{

class CameraEntity:public Entity{
public:
	CameraEntity(Engine *engine);
	virtual ~CameraEntity();
	virtual void destroy();

	void setProjectionFovX(float fovx,float aspect,float nearDistance,float farDistance);
	void setProjectionFovY(float fovy,float aspect,float nearDistance,float farDistance);

	void setLookAt(const toadlet::egg::math::Vector3 &pos,const toadlet::egg::math::Vector3 &dir,const toadlet::egg::math::Vector3 &up);

	void setViewport(int x,int y,int width,int height);
};

}
}
}
