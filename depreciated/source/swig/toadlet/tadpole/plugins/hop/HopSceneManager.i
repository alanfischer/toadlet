%{
#include <toadlet/tadpole/plugins/hop/HopSceneManager.h>
%}

namespace toadlet{
namespace tadpole{

class HopSceneManager:public toadlet::tadpole::sg::SceneManager{
public:
	HopSceneManager(Engine *engine);
	virtual ~HopSceneManager();
	virtual void destroy();

	virtual void setFluidVelocity(const toadlet::egg::math::Vector3 &fluidVelocity);
	virtual const toadlet::egg::math::Vector3 &getFluidVelocity() const;

	virtual void setGravity(const toadlet::egg::math::Vector3 &gravity);
	virtual const toadlet::egg::math::Vector3 &getGravity() const;

	virtual void setRenderCollisionVolumes(bool volumes,bool interpolate,int layer);
};

}
}
