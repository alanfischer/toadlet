%{
#	include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/plugins/glrenderdevice/platform/jgl/JGLWindowREnderTarget.h>
%}

namespace toadlet{
namespace peeper{

class RenderTarget;
/// @todo: set swigCPtr,swigCMemOwn when I create a JObject
%typemap(in) RenderTarget * "$1=($input!=NULL?*(toadlet::peeper::RenderTarget**)&$input:new toadlet::peeper::JGLWindowRenderTarget(jenv,$input_));"

%refobject RenderDevice "$this->retain();"
%unrefobject RenderDevice "$this->release();"

SWIG_JAVABODY_PROXY(public, public, RenderDevice)

class RenderDevice{
public:
	virtual ~RenderDevice();

	virtual bool create(RenderTarget *target)=0;
	virtual void destroy()=0;

	virtual void swap()=0;
	virtual void beginScene()=0;
	virtual void endScene()=0;
};

}
}
