%{
#	include <toadlet/peeper/plugins/glrenderdevice/GLRenderTarget.h>
%}

namespace toadlet{
namespace peeper{

#%feature("director") GLRenderTarget;

class GLRenderTarget:public RenderTarget{
public:
	virtual ~GLRenderTarget();

	virtual void destroy();

	virtual bool isPrimary();
	virtual bool isValid();
	virtual int getWidth();
	virtual int getHeight();

	virtual bool swap();
	virtual void reset();
	virtual bool activate();
	virtual bool deactivate();
	virtual bool activateAdditionalContext();
	virtual void deactivateAdditionalContext();
};

}
}
