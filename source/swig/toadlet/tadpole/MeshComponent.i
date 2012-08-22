%{
#include <toadlet/tadpole/MeshComponent.h>
%}

namespace toadlet{
namespace tadpole{

class MeshComponent:public Component{
public:
	MeshComponent(Engine *engine);
	void destroy();

	void setName(String name);
	String getName() const;
	
	bool getActive() const;
	
	void setMesh(String name);

	Material *getSubMaterial(int i);
	Material *getSubMaterial(String name);
	
	RenderState *getSharedRenderState();
};

}
}
