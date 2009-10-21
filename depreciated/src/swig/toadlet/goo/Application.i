%{
#include <toadlet/goo/Application.h>
%}

namespace toadlet{
namespace goo{

%newobject Application::newApplication(toadlet::tadpole::Engine *engine);

class Application{
public:
	static Application *newApplication(toadlet::tadpole::Engine *engine);

	virtual bool start(bool runEventLoop)=0;
	virtual void stepEventLoop()=0;
	virtual bool stop(bool terminate)=0;
	virtual bool isRunning() const=0;
	virtual bool isActive() const=0;

	virtual void setTitle(const char *title)=0;
	//virtual const char *getTitle() const=0;

	virtual void setPosition(int x,int y)=0;
	virtual int getPositionX() const=0;
	virtual int getPositionY() const=0;

	virtual void setSize(unsigned int width,unsigned int height)=0;
	virtual unsigned int getWidth() const=0;
	virtual unsigned int getHeight() const=0;

	virtual void setFullscreen(bool fullscreen)=0;
	virtual bool getFullscreen() const=0;

	//virtual void setVisual(const Visual &visual);
	//virtual const Visual &getVisual() const;

	virtual toadlet::tadpole::Engine *getEngine() const=0;
	//virtual toadlet::goo::Painter *getPainter() const=0;
	virtual toadlet::peeper::Renderer *getRenderer() const=0;
};

}
}
