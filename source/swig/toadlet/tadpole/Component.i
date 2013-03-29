%{
#include <toadlet/tadpole/Component.h>
%}

namespace toadlet{
namespace tadpole{

%refobject Component "$this->retain();"
%unrefobject Component "$this->release();"

class Component{
public:
	virtual ~Component(){}

	virtual void destroy()=0;
	
	virtual void setName(String name)=0;
	virtual String getName() const=0;
	
	virtual bool getActive() const=0;
};

}
}
