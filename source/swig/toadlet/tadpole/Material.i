%{
#include <toadlet/tadpole/material/Material.h>
%}

namespace toadlet{
namespace tadpole{

class Material{
public:
	virtual void destroy()=0;

	RenderState *getRenderState();
};

}
}
