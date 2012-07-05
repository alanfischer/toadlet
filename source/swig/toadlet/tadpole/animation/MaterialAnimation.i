%{
#include <toadlet/tadpole/animation/MaterialAnimation.h>
%}

namespace toadlet{
namespace tadpole{
namespace animation{

class MaterialAnimation:public Animation{
public:
	MaterialAnimation(Material *target,Sequence *sequence,int trackIndex=0);

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