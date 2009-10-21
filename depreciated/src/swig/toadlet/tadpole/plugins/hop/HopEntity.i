%{
#include <toadlet/tadpole/plugins/hop/HopEntity.h>
%}

namespace toadlet{
namespace tadpole{

class HopEntity:public toadlet::tadpole::sg::NodeEntity{
public:
	HopEntity(Engine *engine);
	virtual ~HopEntity();
	virtual void destroy();

	void setMass(float mass);
	float getMass() const;

	void setInfiniteMass();
	bool hasInfiniteMass() const;

	void setTranslate(const toadlet::egg::math::Vector3 &translate);
	void setTranslate(float x,float y,float z);
	void setPosition(const toadlet::egg::math::Vector3 &position);
	const toadlet::egg::math::Vector3 &getPosition();

	void setVelocity(const toadlet::egg::math::Vector3 &velocity);
	const toadlet::egg::math::Vector3 &getVelocity() const;

	void addForce(const toadlet::egg::math::Vector3 &force);
	const toadlet::egg::math::Vector3 &getForce() const;
	void clearForce();

	void setCoefficientOfRestitution(float coeff);
	float getCoefficientOfRestitution() const;

	void setCoefficientOfRestitutionOverride(bool over);
	bool getCoefficientOfRestitutionOverride() const;

	void setCoefficientOfStaticFriction(float coeff);
	float getCoefficientOfStaticFriction() const;

	void setCoefficientOfDynamicFriction(float coeff);
	float getCoefficientOfDynamicFriction() const;

	void setCoefficientOfGravity(float coeff);
	float getCoefficientOfGravity() const;

	void setCoefficientOfEffectiveDrag(float coeff);
	float getCoefficientOfEffectiveDrag() const;

	HopSceneManager *getSceneManager() const;
	toadlet::hop::Solid *getSolid() const;
};

}
}
