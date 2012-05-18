%{
#	include <toadlet/peeper/WindowRenderTargetFormat.h>
%}

namespace toadlet{
namespace peeper{

class WindowRenderTargetFormat{
public:
	virtual ~WindowRenderTargetFormat();

	int getPixelFormat();
	int getDepthBits();
	int getStencilBits();
	int getMultisamples();
	bool getVSync();
	int getThreads();
	bool getDebug();
	int getFlags();
};

}
}
