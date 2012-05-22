%{
#	include <toadlet/peeper/RenderTarget.h>
%}

namespace toadlet{
namespace peeper{

%refobject RenderTarget "$this->retain();"
%unrefobject RenderTarget "$this->release();"

class RenderTarget{
public:
	virtual ~RenderTarget();

	virtual void destroy()=0;

	virtual bool isPrimary()=0;
	virtual bool isValid()=0;
	virtual int getWidth()=0;
	virtual int getHeight()=0;
};

}
}
