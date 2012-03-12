%{
#	include <toadlet/peeper/RenderDevice.h>
%}

namespace toadlet{
namespace peeper{

%refobject RenderDevice "$this->retain();"
%unrefobject RenderDevice "$this->release();"

class RenderDevice{
public:
	virtual bool create(RenderTarget *target,int *options)=0;
	virtual void destroy()=0;

	virtual void swap()=0;
	virtual void beginScene()=0;
	virtual void endScene()=0;
};

}
}

%inline %{
extern "C" toadlet::peeper::RenderDevice *new_GLRenderDevice();
%}
