%{
#include <toadlet/tadpole/animation/MeshAnimation.h>
%}

namespace toadlet{
namespace tadpole{
namespace animation{

class MeshAnimation:public Animation{
public:
	MeshAnimation(MeshNode *target,int sequenceIndex);

	void setValue(scalar value);
	scalar getMinValue() const;
	scalar getMaxValue() const;
	scalar getValue() const;

	void setWeight(scalar weight);
	scalar getWeight() const;
};

}
}
}
