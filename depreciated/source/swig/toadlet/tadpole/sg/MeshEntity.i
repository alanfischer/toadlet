%{
#include <toadlet/tadpole/sg/MeshEntity.h>
%}

namespace toadlet{
namespace tadpole{
namespace sg{

class MeshEntity:public Entity{
public:
	MeshEntity(Engine *engine);
	virtual ~MeshEntity();
	virtual void destroy();

	void load(const char *name);

	void startAnimation(int animation,bool loop,float timeScale=1.0,bool keepLastFrame=false);
	void stopAnimation();
};

}
}
}
