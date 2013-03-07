%{
#	include <toadlet/flick/InputDevice.h>
%}

namespace toadlet{
namespace flick{

%refobject InputDevice "$this->retain();"
%unrefobject InputDevice "$this->release();"

class InputDevice{
public:
	enum InputType{
		JOY=0,
		LINEAR=1,
		ANGULAR=2,
		LIGHT=3,
		PROXIMITY=4,
		MAGNETIC=5,
		MAX=6,
	};

	virtual ~InputDevice();

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual InputType getType()=0;
	virtual bool start()=0;
	virtual void update(int dt)=0;
	virtual void stop()=0;
};

}
}
