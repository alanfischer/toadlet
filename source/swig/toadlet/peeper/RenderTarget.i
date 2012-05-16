%{
#	include <toadlet/peeper/RenderTarget.h>
%}

namespace toadlet{
namespace peeper{

%refobject RenderTarget "$this->retain();"
%unrefobject RenderTarget "$this->release();"

class RenderTarget{
public:
	void destroy()=0;

	bool isValid()=0;
};

}
}
/*
%inline %{
extern "C" toadlet::peeper::RenderTarget *new_WGLWindowRenderTarget(int display,int window,int format);
%}
*/