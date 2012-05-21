%{
#	include <toadlet/flick/InputDeviceListener.h>
%}

namespace toadlet{
namespace flick{

#%feature("director") InputDeviceListener;

class InputDeviceListener{
public:
	virtual ~InputDeviceListener();

	virtual void inputDetected(const InputData &data)=0;
};

}
}
