%{
#include <toadlet/hop/Solid.h>
#include <toadlet/hop/Shape.h>
%}

namespace toadlet{
namespace hop{

%feature("pythonappend") Solid::addShape
%{args[0].thisown=0%}

class Solid{
public:
	enum Interaction{
		INTERACTION_NORMAL=0,
		INTERACTION_NONE,
		INTERACTION_GHOST,
	};

	Solid();
	virtual ~Solid();

	void setInteraction(Interaction i);
	Interaction getInteraction() const;

	void setMass(float mass);
	float getMass() const;

	void setInfiniteMass();
	bool hasInfiniteMass() const;

	void setPosition(const Vector3 &position);
	const Vector3 &getPosition() const;

	void setVelocity(const Vector3 &velocity);
	const Vector3 &getVelocity() const;

	void addForce(const Vector3 &force);
	const Vector3 &getForce() const;
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

	void addShape(toadlet::hop::Shape *shape);
	void removeShape(toadlet::hop::Shape *shape);
	toadlet::hop::Shape *getShape(int i);
	const toadlet::hop::Shape *getShape(int i) const;
	int getNumShapes() const;
};

}
}
