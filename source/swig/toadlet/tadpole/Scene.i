%{
#include <toadlet/tadpole/Scene.h>
%}

namespace toadlet{
namespace tadpole{

%refobject Scene "$this->retain();"
%unrefobject Scene "$this->release();"
%newobject Scene::getRoot();

class Scene{
public:
	Scene(Engine *engine);
	void destroy();

	Engine *getEngine();

	toadlet::tadpole::Node *getRoot();

	void update(int dt);
};

}
}
