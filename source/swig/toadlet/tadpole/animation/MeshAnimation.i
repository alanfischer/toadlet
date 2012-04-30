%{
#include <toadlet/tadpole/animation/MeshAnimation.h>
%}

namespace toadlet{
namespace tadpole{
namespace animation{

class MeshAnimation:public Animation{
public:
	MeshAnimation(MeshNode *target,int sequenceIndex);

	virtual void setValue(scalar value);
	virtual scalar getMinValue() const;
	virtual scalar getMaxValue() const;
	virtual scalar getValue() const;

	virtual void setWeight(scalar weight);
	virtual scalar getWeight() const;
};

}
}
}