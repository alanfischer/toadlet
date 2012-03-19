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
	void setValue(float value)=0;
	float getMinValue()=0;
	float getMaxValue()=0;
	float getValue()=0;

	void setWeight(float weight)=0;
	float getWeight() const=0;
};

}
}
}
