%{
#	include <toadlet/tadpole/Engine.h>
%}

namespace toadlet{
namespace tadpole{

%refobject Engine "$this->retain();"
%unrefobject Engine "$this->release();"

class Engine{
public:
	void installHandlers();
	bool setRenderDevice(toadlet::peeper::RenderDevice *renderDevice);

	toadlet::tadpole::Node *createNode(char *fullName,toadlet::tadpole::Scene *scene);
};

}
}
