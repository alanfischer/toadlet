%{
#include <toadlet/tadpole/node/MeshNode.h>
%}

namespace toadlet{
namespace tadpole{

class MeshNode:public Node{
public:
	Node *create(Scene *scene);
	void destroy();

	void setMesh(char *name);

	Material *getSubMaterial(int i);
	Material *getSubMaterial(char *name);
	
	RenderState *getSharedRenderState();
};

}
}
