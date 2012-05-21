%{
#	include <toadlet/pad/Applet.h>
%}

namespace toadlet{
namespace pad{

%refobject Applet "$this->retain();"
%unrefobject Applet "$this->release();"

class Applet{
public:
	virtual ~Applet();

	virtual void create()=0;
	virtual void destroy()=0;
	virtual void update(int dt)=0;
	virtual void render()=0;

	virtual void resized(int width,int height)=0;
	virtual void focusGained()=0;
	virtual void focusLost()=0;

	virtual void keyPressed(int key)=0;
	virtual void keyReleased(int key)=0;

	virtual void mousePressed(int x,int y,int button)=0;
	virtual void mouseMoved(int x,int y)=0;
	virtual void mouseReleased(int x,int y,int button)=0;
	virtual void mouseScrolled(int x,int y,int scroll)=0;
};

}
}
