%{
#include <toadlet/tadpole/sg/LightEntity.h>
%}

namespace toadlet{
namespace tadpole{
namespace sg{

class LightEntity:public Entity{
public:
	LightEntity(Engine *engine);
	virtual ~LightEntity();
	virtual void destroy();

	inline void setDirection(const toadlet::egg::math::Vector3 &direction);
	inline const toadlet::egg::math::Vector3 &getDirection() const;
};

}
}
}
