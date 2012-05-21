%{
#include <toadlet/tadpole/node/CameraNode.h>
%}

namespace toadlet{
namespace tadpole{

class CameraNode:public Node{
public:
	Node *create(Scene *scene);
	void destroy();

	void setClearColor(float color[4]);

	void setLookAt(float eye[3],float point[3],float up[3]);

	void setProjectionFovY(float fov,float ratio,float nearDist,float farDist);
	void setProjectionFovX(float fov,float ratio,float nearDist,float farDist);

	void render(RenderDevice *renderDevice);
};

}
}
