%{
#include <toadlet/tadpole/animation/BaseAnimation.h>
%}

namespace toadlet{
namespace tadpole{
namespace animation{

%feature("director") BaseAnimation;

class BaseAnimation:public Animation{
public:
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