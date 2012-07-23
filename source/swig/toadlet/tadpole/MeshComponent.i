%{
#include <toadlet/tadpole/MeshComponent.h>
%}

namespace toadlet{
namespace tadpole{

class MeshComponent:public Component{
public:
	void destroy();

	void setMesh(String name);

	Material *getSubMaterial(int i);
	Material *getSubMaterial(String name);
	
	RenderState *getSharedRenderState();
};

}
}
