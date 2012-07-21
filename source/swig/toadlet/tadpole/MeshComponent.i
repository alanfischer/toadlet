%{
#include <toadlet/tadpole/MeshComponent.h>
%}

namespace toadlet{
namespace tadpole{

class MeshComponent:public Component{
public:
	void destroy();

	void setMesh(char *name);

	Material *getSubMaterial(int i);
	Material *getSubMaterial(char *name);
	
	RenderState *getSharedRenderState();
};

}
}
