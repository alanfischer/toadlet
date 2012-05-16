%{
#	include <toadlet/flick/InputDevice.h>
%}

namespace toadlet{
namespace flick{

%refobject InputDevice "$this->retain();"
%unrefobject InputDevice "$this->release();"

class InputDevice{
public:
	virtual ~InputDevice();

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual void update(int dt)=0;
};

}
}
