%{
#	include <toadlet/tadpole/Engine.h>
%}

namespace toadlet{
namespace tadpole{

%newobject Engine::newEngineWithAllHandlers();
%newobject Engine::newEngineWithMinimumHandlers();
%newobject Engine::newEngineWithNoHandlers();

class Engine{
public:
	static Engine *newEngineWithAllHandlers();
	static Engine *newEngineWithMinimumHandlers();
	static Engine *newEngineWithNoHandlers();

	void setSceneManager(toadlet::tadpole::sg::SceneManager *sceneManager);
	toadlet::tadpole::sg::SceneManager *getSceneManager() const;

	void cacheMesh(const char *name);

protected:
	Engine();
};

}
}
