%{
#include <toadlet/tadpole/animation/Animation.h>
%}

namespace toadlet{
namespace tadpole{
namespace animation{

%refobject Animation "$this->retain();"
%unrefobject Animation "$this->release();"

class Animation{
public:
	virtual ~Animation(){}

	virtual String getName() const=0;
	
	virtual void setValue(scalar value)=0;
	virtual scalar getMinValue() const=0;
	virtual scalar getMaxValue() const=0;
	virtual scalar getValue() const=0;

	virtual void setWeight(scalar weight)=0;
	virtual scalar getWeight() const=0;
	
	virtual void setScope(int scope)=0;
	virtual int getScope() const=0;
};

}
}
}
