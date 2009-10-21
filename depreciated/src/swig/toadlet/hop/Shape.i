%{
#include <toadlet/hop/Shape.h>
%}

namespace toadlet{
namespace hop{

class Shape{
public:
	enum Type{
		TYPE_AABOX=0,
		TYPE_SPHERE,
		TYPE_CAPSULE,
	};

	Shape();
	Shape(const egg::math::AABox &box);
	virtual ~Shape();

	void setAABox(const AABox &box);
	inline const AABox &getAABox() const;
};

}
}
