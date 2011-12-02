%{
#include <toadlet/egg/math/Vector3.h>
%}

namespace toadlet{
namespace egg{
namespace math{

class AABox{
public:
	inline AABox(const Vector3 &min,const Vector3 &max);

	inline AABox(float minx,float miny,float minz,float maxx,float maxy,float maxz);
};

}
}
}
