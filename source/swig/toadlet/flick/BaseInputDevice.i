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

	virtual void update(int dt);
};

}
}
