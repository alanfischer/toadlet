%{
#include <toadlet/flick/InputDeviceListener.h>
%}

namespace toadlet{
namespace flick{

%typemap(javainterfaces) BaseInputDeviceListener "InputDeviceListener"

%{
namespace toadlet{
namespace flick{
	typedef InputDeviceListener BaseInputDeviceListener;
}
}
%}

class BaseInputDeviceListener{
public:
	virtual ~BaseInputDeviceListener(){}

	virtual void inputDetected(const InputData &data)=0;
};

}
}
