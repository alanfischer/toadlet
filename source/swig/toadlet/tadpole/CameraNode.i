%{
#include <toadlet/tadpole/Node/CameraNode.h>
%}

namespace toadlet{
namespace tadpole{

class CameraNode:public Node{
public:
	Node *create(Scene *scene);
	void destroy();

	void setClearColor(float color[4]);

	void setLookAt(float eye[4],float point[4],float up[4]);

	void setProjectionFovY(float fov,float ratio,float nearDist,float farDist);
	void setProjectionFovX(float fov,float ratio,float nearDist,float farDist);

	void render(peeper::RenderDevice *renderDevice);
};

}
}
