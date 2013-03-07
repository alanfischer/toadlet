%{
#	include <toadlet/flick/BaseInputDevice.h>
%}

namespace toadlet{
namespace flick{

#%feature("director") BaseInputDevice;

class BaseInputDevice:public InputDevice{
public:
	virtual ~BaseInputDevice();

	virtual bool create();
	virtual void destroy();

	virtual InputType getType();
	virtual bool start();
	virtual void update(int dt);
	virtual void stop();
};

}
}
