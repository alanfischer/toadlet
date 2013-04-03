%{
#include <toadlet/tadpole/action/BaseAnimation.h>
%}

namespace toadlet{
namespace tadpole{
namespace action{

#%feature("director") BaseAnimation;

class BaseAnimation:public Animation{
public:
	virtual String getName() const;

	virtual void setValue(scalar value);
	virtual scalar getMinValue() const;
	virtual scalar getMaxValue() const;
	virtual scalar getValue() const;

	virtual void setWeight(scalar weight);
	virtual scalar getWeight() const;
	
	virtual void setScope(int scope);
	virtual int getScope() const;
};

}
}
}
