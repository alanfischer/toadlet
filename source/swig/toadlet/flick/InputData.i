%{
#	include <toadlet/flick/InputData.h>
%}

%javaconst(1);

namespace toadlet{
namespace flick{

class InputData{
public:
	static const int Semantic_JOY_BUTTON_PRESSED=0;
	static const int Semantic_JOY_BUTTON_RELEASED=1;
	static const int Semantic_JOY_DIRECTION=2;
	static const int Semantic_JOY_ROTATION=3;
	static const int Semantic_MAX_JOY=4;
		
	static const int Semantic_MOTION_ACCELERATION=0;
	static const int Semantic_MOTION_VELOCITY=1;
	static const int Semantic_MAX_MOTION=2;

	static const int Semantic_ANGULAR=0;
	static const int Semantic_MAX_ANGULAR=1;
		
	static const int Semantic_LIGHT=0;
	static const int Semantic_MAX_LIGHT=1;

	static const int Semantic_PROXIMITY=0;
	static const int Semantic_MAX_PROXIMITY=1;

	static const int Semantic_MAGNETIC=0;
	static const int Semantic_MAX_MAGNETIC=1;

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
