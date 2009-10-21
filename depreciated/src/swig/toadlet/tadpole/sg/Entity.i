%{
#include <toadlet/tadpole/sg/Entity.h>
%}

namespace toadlet{
namespace tadpole{
namespace sg{

%refobject Entity "$this->getCounter()->incSharedCount();"
%unrefobject Entity "if($this->getCounter()->decSharedCount()<1){delete $this;}"

class Entity{
public:
	Entity(Engine *engine);
	virtual ~Entity();
	virtual void destroy();

	inline bool instanceOf(toadlet::uint64 type) const;
	inline toadlet::uint64 getType() const;
	inline bool destroyed() const;

	virtual void parentChanged(toadlet::tadpole::sg::Entity *entity,toadlet::tadpole::sg::NodeEntity *parent);
	inline toadlet::tadpole::sg::NodeEntity *getParent() const;

	virtual void setTranslate(const toadlet::egg::math::Vector3 &translate);
	virtual void setTranslate(float x,float y,float z);
	inline const toadlet::egg::math::Vector3 &getTranslate() const;

	virtual void setRotate(const toadlet::egg::math::Matrix3x3 &rotate);
	virtual void setRotate(float x,float y,float z,float angle);
	inline const toadlet::egg::math::Matrix3x3 &getRotate() const;

	virtual void setScale(const toadlet::egg::math::Vector3 &scale);
	virtual void setScale(float x,float y,float z);
	inline const toadlet::egg::math::Vector3 &getScale() const;

	inline bool isIdentityTransform() const;

	virtual void setScope(int scope);
	inline int getScope() const;

	virtual void setBoundingRadius(float boundingRadius);
	inline float getBoundingRadius() const;

	inline Engine *getEngine() const;
};

}
}
}
