%{
#include <toadlet/tadpole/animation/MeshAnimation.h>
%}

namespace toadlet{
namespace tadpole{
namespace animation{

class MeshAnimation:public Animation{
public:
	MeshAnimation(MeshComponent *target,int sequenceIndex);

	String getName() const;

	void setValue(scalar value);
	scalar getMinValue() const;
	scalar getMaxValue() const;
	scalar getValue() const;

	void setWeight(scalar weight);
	scalar getWeight() const;
	
	void setScope(int scope);
	int getScope() const;
};

}
}
}
