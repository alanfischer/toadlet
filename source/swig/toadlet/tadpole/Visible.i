%{
#include <toadlet/tadpole/Visible.h>
%}

namespace toadlet{
namespace tadpole{

class Visible{
public:
	virtual ~Visible(){}

	virtual bool getRendered() const=0;
	virtual void setRendered(bool visible)=0;
	virtual RenderState *getSharedRenderState()=0;
};

}
}
