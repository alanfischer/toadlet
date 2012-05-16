%{
#	include <toadlet/flick/InputData.h>
%}

namespace toadlet{
namespace flick{

class InputData{
public:
	enum Semantic{
		Semantic_JOY_BUTTON_PRESSED=0,
		Semantic_JOY_BUTTON_RELEASED,
		Semantic_JOY_DIRECTION,
		Semantic_JOY_ROTATION,
		Semantic_MAX_JOY,
		
		Semantic_MOTION_ACCELERATION=0,
		Semantic_MOTION_VELOCITY,
		Semantic_MAX_MOTION,

		Semantic_ANGULAR=0,
		Semantic_MAX_ANGULAR,
		
		Semantic_LIGHT=0,
		Semantic_MAX_LIGHT,

		Semantic_PROXIMITY=0,
		Semantic_MAX_PROXIMITY,

		Semantic_MAGNETIC=0,
		Semantic_MAX_MAGNETIC,
	};

	InputData(int type1=0,int time1=0,int size=0);
	void set(const InputData &data);
};

}
}
