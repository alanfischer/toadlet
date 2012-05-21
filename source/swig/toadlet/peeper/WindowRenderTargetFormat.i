%{
#	include <toadlet/peeper/WindowRenderTargetFormat.h>
%}

namespace toadlet{
namespace peeper{

class WindowRenderTargetFormat{
public:
	virtual ~WindowRenderTargetFormat();

	void setPixelFormat(int format);
	int getPixelFormat();
	
	void setDepthBits(int bits);
	int getDepthBits();
	
	void setStencilBits(int bits);
	int getStencilBits();
	
	void setMultisamples(int samples);
	int getMultisamples();
	
	void setVSync(bool sync);
	bool getVSync();
	
	void setThreads(int threads);
	int getThreads();
	
	void setDebug(bool debug);
	bool getDebug();
	
	void setFlags(int flags);
	int getFlags();
};

}
}
