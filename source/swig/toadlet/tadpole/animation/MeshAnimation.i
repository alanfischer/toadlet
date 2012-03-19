%{
#include <toadlet/tadpole/animation/SkeletonAnimation.h>
%}

namespace toadlet{
namespace tadpole{
namespace animation{

class SkeletonAnimation:public Animation{
public:
	SkeletonAnimation(MeshNode *target,int sequenceIndex);

	void setValue(float value);
	float getMinValue();
	float getMaxValue();
	float getValue();

	void setWeight(float weight);
	float getWeight() const;
};

}
}
}
