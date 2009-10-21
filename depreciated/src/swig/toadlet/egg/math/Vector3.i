%{
#include <toadlet/egg/math/Vector3.h>
%}

namespace toadlet{
namespace egg{
namespace math{

class Vector3{
public:
	float x,y,z;

	inline Vector3(float x1,float y1,float z1);

	inline void set(const Vector3 &v);

	inline void set(float x1,float y1,float z1);

	inline void reset();
};

}
}
}
