%{
#include <toadlet/tadpole/action/MaterialStateAnimation.h>
%}

namespace toadlet{
namespace tadpole{
namespace action{

class MaterialStateAnimation:public Animation{
public:
	MaterialStateAnimation(RenderState *target,Sequence *sequence,int trackIndex=0);

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
