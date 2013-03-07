%{
#	include <toadlet/flick/InputData.h>
%}

namespace toadlet{
namespace flick{

class InputData{
public:
	enum Semantic{
		JOY_BUTTON_PRESSED=0,
		JOY_BUTTON_RELEASED,
		JOY_DIRECTION,
		JOY_ROTATION,
		MAX_JOY,

		LINEAR_ACCELERATION=0,
		LINEAR_VELOCITY,
		MAX_LINEAR,

		ANGULAR=0,
		MAX_ANGULAR,

		LIGHT=0,
		MAX_LIGHT,

		PROXIMITY=0,
		MAX_PROXIMITY,

		MAGNETIC=0,
		MAX_MAGNETIC,
	};

	InputData(int type1=0,int time1=0,int size=0);

	void set(const InputData &data);

	void setType(int type);
	int getType();
	
	void setTime(uint64 time);
	uint64 getTime();
	
	void setValid(int valid);
	int getValid();
	
	void setValue(int i,float value[4]);
//	void getValue(float value[4],int i);
};

}
}
