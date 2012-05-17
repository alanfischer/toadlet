%{
#	include <toadlet/flick/InputDevice.h>
%}

%javaconst(1);
namespace toadlet{
namespace flick{

%refobject InputDevice "$this->retain();"
%unrefobject InputDevice "$this->release();"

class InputDevice{
public:
	static const int InputType_JOY=0;
	static const int InputType_MOTION=1;
	static const int InputType_ANGULAR=2;
	static const int InputType_LIGHT=3;
	static const int InputType_PROXIMITY=4;
	static const int InputType_MAGNETIC=5;
	static const int InputType_MAX=6;

	virtual ~InputDevice();

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual int getType()=0;
	virtual bool start()=0;
	virtual void update(int dt)=0;
	virtual void stop()=0;
};

}
}
